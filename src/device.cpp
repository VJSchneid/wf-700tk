#include <wf700tk/device.hpp>

#include <algorithm>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <wf700tk/request_writer.hpp>

#include <iostream>

namespace wf700tk {

device::device(net::serial_port &&port)
    : port_(std::move(port)), poll_timer_(port_.get_executor()),
      request_buffer_(request_writer{}.length()) {}

void device::input_callback(
    const std::function<void(const device::error_code &, credit_channel)>
        &callback) {
    if (input_callback_) {
        input_callback_(error_code(net::error::operation_aborted),
                        credit_channel::none);
    }
    input_callback_ = callback;
    if (response_timeout_counter_ < 0) {
        // we should inform the new callback also if there is a timeout
        response_timeout_counter_ = -1;
    }
}

void device::output(ticket_channel channel, unsigned int amount,
                    const device::output_handler &handler) {
    if (amount == 0) {
        handler(error_code(), amount);
        return;
    }

    auto index = ticket_channel_index(channel);
    if (!index) {
        handler(net::error::invalid_argument, amount);
        return;
    }

    auto &output = ticket_output_[*index];
    if (!output.active) {
        output.to_dispense = amount;
        output.callback = handler;
    } else {
        handler(net::error::already_started, amount);
    }
}

net::serial_port &device::serial_port() { return port_; }

const net::serial_port &device::serial_port() const { return port_; }

void device::start_poll() {
    poll_timer_.expires_after(polling_duration_);
    poll_timer_.async_wait([this](const error_code &ec) {
        if (!ec) {
            poll();
        }
    });
}

void device::start_read() {
    port_.async_read_some(net::buffer(response_buffer_),
                          [this](const error_code &ec, std::size_t len) {
                              if (!ec) {
                                  handle_read(len);
                              }
                          });
}

void device::handle_read(std::size_t len) {
    response_reader_.put(net::buffer(response_buffer_.data(), len));
    if (response_reader_.success()) {
        handle_poll_response(response_reader_.message());
        response_timeout_counter_ = response_timeout_;
    }
    start_read();
}

void device::handle_poll_response(const response &resp) {
    if (resp.accepted_credit_) {
        input_callback_(error_code(), resp.credit_channel_);
    }

    if (auto i = ticket_channel_index(resp.ticket_channel_)) {
        auto &channel = ticket_output_[*i];
        if (channel.active) {
            channel.inform(resp.tickets_to_dispense_);
        }
    }
}

void device::poll() {
    request_writer req_w;
    req_w.ack(next_ack);
    next_ack = next_ack ? 0 : 1;
    prepare_poll(req_w.msg());
    auto buf = net::buffer(request_buffer_);
    req_w.write(buf);

    if (response_timeout_counter_ >= 0) {
        --response_timeout_counter_;
    } else {
        handle_response_timeout();
    }

    net::async_write(
        port_, buf,
        [this](const error_code &ec, std::size_t /*bytes_written*/) {
            if (!ec) {
                start_poll();
            } else {
                handle_port_error(ec);
            }
        });
}

void device::prepare_poll(request &req) {
    for (unsigned int i = 0; i < ticket_output_.size(); ++i) {
        // TODO: check wether this works. This should stop running outputs that
        // timed out
        auto &output = ticket_output_[i];
        if (output.active && output.timed_out()) {
            req.ticket_channel_ = *make_ticket_channel_from_index(i);
            req.tickets_to_dispense(0);
            if (output.callback) {
                output.callback(error_code(net::error::timed_out),
                                output.to_dispense);
            }
            output.reset();
            return;
        }
    }

    for (unsigned int i = 0; i < ticket_output_.size(); ++i) {
        auto &output = ticket_output_[i];
        if (!output.active && output.to_dispense > 0) {
            req.ticket_channel_ = *make_ticket_channel_from_index(i);
            req.tickets_to_dispense(output.next_dispense_amount());
            output.prepare_for_sending(req.tickets_to_dispense());
            return;
        }
    }
}

void device::handle_response_timeout() {
    error_code ec(net::error::timed_out);
    if (response_timeout_counter_ == -1 && input_callback_) {
        // do not inform this callback anymore
        response_timeout_counter_ = -2;
        input_callback_(ec, credit_channel::none);
    }
    handle_global_output_error(ec);
}

void device::handle_port_error(const device::error_code &ec) {
    port_.close();
    if (input_callback_) {
        input_callback_(ec, credit_channel::none);
    }
    handle_global_output_error(ec);
}

void device::handle_global_output_error(const error_code &ec) {
    for (auto &channel : ticket_output_) {
        if (channel.active) {
            if (channel.callback) {
                channel.callback(ec, channel.to_dispense);
            }
            channel.reset();
        }
    }
}

void device::output_state::inform(unsigned char tickets_to_dispense) {
    if (last_dispense_state > tickets_to_dispense) {
        timeout_counter = 0;

        int dispensed = last_dispense_state - tickets_to_dispense;

        to_dispense = std::max(0, to_dispense - dispensed);

        if (to_dispense == 0) {
            callback(error_code(), 0);
            reset();
        } else if (tickets_to_dispense == 0) {
            active = false;
            timeout_counter = -1; // add timeout bonus to prevent timeouts due
                                  // to transmission times
        }
    } else {
        ++timeout_counter;
    }

    last_dispense_state = tickets_to_dispense;
}

unsigned char device::output_state::next_dispense_amount() const {
    return std::min(static_cast<unsigned char>(to_dispense),
                    static_cast<unsigned char>(0x0f));
}

void device::output_state::prepare_for_sending(unsigned int amount) {
    last_dispense_state = amount;
    active = true;
}

void device::output_state::reset() {
    active = false;
    to_dispense = 0;
    timeout_counter = 0;
    callback = decltype(callback)();
}

bool device::output_state::timed_out() const { return timeout_counter > 2; }

} // namespace wf700tk
