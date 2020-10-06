#ifndef WF700TK_DETAIL_MESSAGE_BASE_HPP
#define WF700TK_DETAIL_MESSAGE_BASE_HPP

#include <array>

namespace wf700tk::detail {

struct message_base {
    message_base(unsigned char length, bool is_request);

    constexpr const static unsigned char start_byte_ = 0x02;
    constexpr const static unsigned char end_byte_ = 0x03;
    constexpr const static unsigned char request_byte_ = 0x10;
    constexpr const static unsigned char response_byte_ = 0x20;
    constexpr const static unsigned int tail_size_ = 2;

    unsigned char length() const;

    const std::array<unsigned char, 3> head_;
};

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_MESSAGE_BASE_HPP
