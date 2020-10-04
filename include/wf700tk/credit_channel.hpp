#ifndef WF700TK_CREDIT_CHANNEL_HPP
#define WF700TK_CREDIT_CHANNEL_HPP

#include <optional>

namespace wf700tk {

enum class credit_channel : unsigned char {
    none = 0x00,
    input_1 = 0x08,
    input_2 = 0x10,
    input_3 = 0x18,
    input_4 = 0x20
};

std::optional<credit_channel> make_credit_channel(unsigned char v);

} // namespace wf700tk

#endif // WF700TK_CREDIT_CHANNEL_HPP
