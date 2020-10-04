#include <wf700tk/response.hpp>

namespace wf700tk {

response::response(bool accepted_credit, credit_channel cc, ticket_channel tc,
                   unsigned char tickets_to_dispense,
                   unsigned char tickets_already_dispensed)
    : accepted_credit_(accepted_credit), credit_channel_(cc),
      ticket_channel_(tc), tickets_to_dispense_(tickets_to_dispense),
      tickets_already_dispensed_(tickets_already_dispensed) {}

} // namespace wf700tk
