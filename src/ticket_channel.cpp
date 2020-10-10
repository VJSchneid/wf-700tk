#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

std::optional<ticket_channel> make_ticket_channel(unsigned int v) {
    if ((v & 0x0f) == 0) {
        if (v <= 0x40) {
            return static_cast<ticket_channel>(v);
        }
    }
    return std::nullopt;
}

std::optional<ticket_channel> make_ticket_channel_from_index(unsigned int i) {
    if (i >= 4) {
        return std::nullopt;
    }
    return static_cast<ticket_channel>((i + 1) << 4);
}

std::optional<unsigned int> ticket_channel_index(ticket_channel v) {
    if (v == ticket_channel::none) {
        return std::nullopt;
    }
    return (static_cast<unsigned int>(v) >> 4) - 1;
}

} // namespace wf700tk
