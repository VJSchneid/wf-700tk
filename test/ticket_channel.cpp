#include <catch2/catch.hpp>

#include <wf700tk/ticket_channel.hpp>

namespace wf700tk {

TEST_CASE("valid ticket channels are handled correctly") {
    CHECK(make_ticket_channel(0x00) == ticket_channel::none);
    CHECK(make_ticket_channel(0x10) == ticket_channel::ticket_1);
    CHECK(make_ticket_channel(0x20) == ticket_channel::ticket_2);
    CHECK(make_ticket_channel(0x30) == ticket_channel::ticket_3);
    CHECK(make_ticket_channel(0x40) == ticket_channel::ticket_4);
}

TEST_CASE("invalid ticket channels are rejected") {
    CHECK(!make_ticket_channel(0x01));
    CHECK(!make_ticket_channel(0x11));
    CHECK(!make_ticket_channel(0x50));
    CHECK(!make_ticket_channel(0xf0));
    CHECK(!make_ticket_channel(0xff));
}

} // namespace wf700tk
