#include <wf700tk/detail/basic_writer.hpp>

namespace wf700tk::detail {

basic_writer::basic_writer(unsigned int msg_length_, bool is_request)
    : message_base(msg_length_, is_request) {}

bool basic_writer::write(const boost::asio::mutable_buffer &buf) {
    auto begin = net::buffer_cast<unsigned char *>(buf);

    auto msg_length = length();
    if (net::buffer_size(buf) < msg_length) {
        return false;
    }

    begin = std::copy(head_.begin(), head_.end(), begin);

    if (!write_data(begin)) {
        return false;
    }

    auto tail = begin + msg_length - head_.size() - tail_size_;
    tail[0] = end_byte_;
    tail[1] = 0x77; // TODO: checksum

    return true;
}

} // namespace wf700tk::detail
