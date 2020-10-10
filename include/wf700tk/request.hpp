#ifndef WF700TK_REQUEST_HPP
#define WF700TK_REQUEST_HPP

#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

struct request {

    inline unsigned char tickets_to_dispense(unsigned char v) {
        tickets_to_dispense_ = v > 0x0f ? 0x0f : v;
        return tickets_to_dispense_;
    }

    inline unsigned char tickets_to_dispense() const {
        return tickets_to_dispense_;
    }

    bool inhibit_acceptors_ = false;
    ticket_channel ticket_channel_ = ticket_channel::none;

private:
    unsigned char tickets_to_dispense_ = 0x00;
};

} // namespace wf700tk

#endif // WF700TK_REQUEST_HPP
