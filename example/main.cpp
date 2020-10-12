#include <iostream>

#include <wf700tk/device.hpp>

namespace net = boost::asio;

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: example_wf700tk [serialport]\n";
        return 1;
    }

    using boost::system::error_code;
    net::io_context ioc;

    wf700tk::device dev(net::serial_port(ioc, argv[1]));

    dev.input_callback([](const error_code &ec, wf700tk::credit_channel ch) {
        std::clog << "received credit input from channel: "
                  << static_cast<int>(ch) << " (" << ec.message() << ")\n";
    });

    dev.output(wf700tk::ticket_channel::ticket_3, 1,
               [](const error_code &ec, int left) {
                   std::clog << "coin output: " << ec.message() << "\n";
                   if (ec) {
                       std::clog << "missing tickets: " << left << '\n';
                   }
               });

    dev.start();

    ioc.run();
}
