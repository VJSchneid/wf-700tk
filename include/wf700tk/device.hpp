#ifndef WF700TK_DEVICE_HPP
#define WF700TK_DEVICE_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <system_error>

#include <wf700tk/response_parser.hpp>
#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

namespace net = boost::asio;

class request;

// TODO: error handling
class device {
public:
    using error_code = boost::system::error_code;
    using clock = std::chrono::steady_clock;
    using output_handler =
        std::function<void(const error_code &, unsigned int not_dispensed)>;

    device(net::serial_port &&port);

    inline void start() {
        start_poll();
        start_read();
    }

    inline void polling_duration(const clock::duration &v) {
        polling_duration_ = v;
    }

    inline clock::duration polling_duration() const {
        return polling_duration_;
    }

    void input_callback(const std::function<void(const error_code &ec,
                                                 credit_channel)> &callback);

    void output(ticket_channel channel, unsigned int amount,
                const output_handler &handler);

    net::serial_port &serial_port();
    const net::serial_port &serial_port() const;

private:
    void start_poll();

    void start_read();
    void handle_read(std::size_t len);
    void handle_poll_response(const response &resp);

    void poll();

    void prepare_poll(request &req);

    net::serial_port port_;

    net::steady_timer poll_timer_;
    clock::duration polling_duration_ = std::chrono::milliseconds(200);

    std::vector<unsigned char> request_buffer_;
    std::vector<unsigned char> response_buffer_{32};
    response_parser response_reader_;

    struct output_state {
        bool active = false;
        int to_dispense = 0;
        output_handler callback;

        void inform(unsigned char tickets_dispensed);
        unsigned char next_dispense_amount() const;
        void prepare_for_sending(unsigned int amount);
        void reset();
        bool timed_out() const;

    private:
        unsigned char last_dispense_state = 0x00;
        int timeout_counter = 0;
    };

    std::array<output_state, 4> ticket_output_;

    std::function<void(const error_code &, credit_channel)> input_callback_;
};

} // namespace wf700tk

#endif // WF700TK_DEVICE_HPP
