#include <catch2/catch.hpp>

#include <wf700tk/request.hpp>

namespace wf700tk {

TEST_CASE("request: correct invalid tickets to dispense") {
    request req;
    CHECK(15 == req.tickets_to_dispense(200));
    CHECK(req.tickets_to_dispense() == 15);
}

TEST_CASE("request: default initialized request is poll request") {
    request req;
    CHECK(req.inhibit_acceptors_ == false);
    CHECK(req.ticket_channel_ == ticket_channel::none);
    CHECK(req.tickets_to_dispense() == 0);
}

} // namespace wf700tk
