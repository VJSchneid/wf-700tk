#include <wf700tk/credit_channel.hpp>

namespace wf700tk {

std::optional<credit_channel> make_credit_channel(unsigned char v) {
    if (v <= 0x20) {
        const unsigned char lower = v & 0x0f;
        if (lower == 0x08 || lower == 0x00) {
            return static_cast<credit_channel>(v);
        }
    }

    return std::nullopt;
}

} // namespace wf700tk
