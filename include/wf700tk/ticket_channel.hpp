#ifndef WF700TK_TOKEN_CHANNEL_HPP
#define WF700TK_TOKEN_CHANNEL_HPP

#include <optional>

namespace wf700tk {

enum class ticket_channel : unsigned char {
    none = 0x00,
    ticket_1 = 0x10,
    ticket_2 = 0x20,
    ticket_3 = 0x30,
    ticket_4 = 0x40
};

std::optional<ticket_channel> make_ticket_channel(unsigned int v);
std::optional<ticket_channel> make_ticket_channel_from_index(unsigned int i);

std::optional<unsigned int> ticket_channel_index(ticket_channel v);

} // namespace wf700tk

#endif // WF700TK_TOKEN_CHANNEL_HPP
