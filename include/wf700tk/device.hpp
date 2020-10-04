#ifndef WF700TK_DEVICE_HPP
#define WF700TK_DEVICE_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <functional>
#include <system_error>

#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

namespace net = boost::asio;

class device {
public:
    using error_code = boost::system::error_code;
    using clock = std::chrono::steady_clock;
    using output_handler = std::function<void(int, const error_code &)>;

    device(net::serial_port &&port);

    void start(const std::function<void(const error_code &)> &func);

    bool enable_input() const;
    void enable_input(bool);

    void polling_duration(const clock::duration &v);
    clock::duration polling_duration() const;

    void output_timeout(const clock::duration &v);
    clock::duration output_timeout() const;

    void input_callback(std::function<void()>);

    void output(ticket_channel channel, unsigned int amount,
                const output_handler &handler);

private:
    void read();

    net::serial_port port_;
};

} // namespace wf700tk

#endif // WF700TK_DEVICE_HPP
