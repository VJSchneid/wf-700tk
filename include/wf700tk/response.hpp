#ifndef WF700TK_RESPONSE_HPP
#define WF700TK_RESPONSE_HPP

#include <wf700tk/credit_channel.hpp>
#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

struct response {
    response() = default;

    response(bool accepted_credit, credit_channel cc, ticket_channel tc,
             unsigned char tickets_to_dispense,
             unsigned char tickets_already_dispensed);

    bool accepted_credit_ = false;
    credit_channel credit_channel_ = credit_channel::none;
    ticket_channel ticket_channel_ = ticket_channel::none;
    unsigned char tickets_to_dispense_ = 0x00;
    unsigned char tickets_already_dispensed_ = 0x00;
};

} // namespace wf700tk

#endif // WF700TK_RESPONSE_HPP
