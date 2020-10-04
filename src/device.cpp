#include <wf700tk/device.hpp>

#include <boost/asio/read_until.hpp>

namespace wf700tk {

device::device(net::serial_port &&port) : port_(std::move(port)) {}

void device::read() {}

} // namespace wf700tk
