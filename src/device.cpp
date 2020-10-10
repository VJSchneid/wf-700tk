#include <wf700tk/device.hpp>

#include <algorithm>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include <wf700tk/request_writer.hpp>

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
}

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
    }
}

void device::handle_poll_response(const response &resp) {
    if (resp.accepted_credit_) {
        input_callback_(error_code(), resp.credit_channel_);
    }

    if (auto i = ticket_channel_index(resp.ticket_channel_)) {
        auto &channel = ticket_output_[*i];
        if (channel.active) {
            channel.inform(resp.tickets_already_dispensed_);
        }
    }
}

void device::poll() {
    request_writer req_w;
    prepare_poll(req_w.msg());
    auto buf = net::buffer(request_buffer_);
    req_w.write(buf);

    net::async_write(
        port_, buf,
        [this](const error_code &ec, std::size_t /*bytes_written*/) {
            if (!ec) {
                start_poll();
            }
        });
}

void device::prepare_poll(request &req) {
    for (unsigned int i = 0; i < ticket_output_.size(); ++i) {
        // TODO: check wether this works. This should stop running outputs that
        // timed out
        auto &output = ticket_output_[i];
        if (output.timed_out()) {
            req.ticket_channel_ = *make_ticket_channel_from_index(i);
            req.tickets_to_dispense(0);
            output.callback(error_code(net::error::timed_out),
                            output.to_dispense);
            output.reset();
            return;
        }
    }

    for (unsigned int i = 0; i < ticket_output_.size(); ++i) {
        auto &output = ticket_output_[i];
        if (!output.active && output.to_dispense > 0) {
            req.ticket_channel_ = *make_ticket_channel_from_index(i);
            req.tickets_to_dispense(output.next_dispense_amount());
            output.prepare_for_sending();
            return;
        }
    }
}

void device::output_state::inform(unsigned char tickets_dispensed) {
    if (last_dispense_state < tickets_dispensed) {
        timeout_counter = 0;

        to_dispense =
            std::max(0, to_dispense - last_dispense_state - tickets_dispensed);

        if (to_dispense == 0) {
            callback(error_code(), 0);
            reset();
        }
    } else {
        ++timeout_counter;
    }
    last_dispense_state = tickets_dispensed;
}

unsigned char device::output_state::next_dispense_amount() const {
    return std::min(static_cast<unsigned char>(to_dispense),
                    static_cast<unsigned char>(0x0f));
}

void device::output_state::prepare_for_sending() {
    last_dispense_state = 0x00;
    active = true;
}

void device::output_state::reset() {
    active = false;
    to_dispense = 0;
    callback = decltype(callback)();
}

bool device::output_state::timed_out() const { return timeout_counter > 5; }

} // namespace wf700tk
