#include <wf700tk/request_writer.hpp>

namespace wf700tk {

request_writer::request_writer() : basic_writer(8, true) {}

bool request_writer::write_data(unsigned char *data) {
    data[0] = msg_.inhibit_acceptors_ ? 0x00 : 0x03;
    data[1] = static_cast<unsigned char>(msg_.ticket_channel_) |
              msg_.tickets_to_dispense();
    data[2] = 0x00;
    return true;
}

} // namespace wf700tk
