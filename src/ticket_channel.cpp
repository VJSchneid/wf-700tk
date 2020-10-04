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

} // namespace wf700tk
