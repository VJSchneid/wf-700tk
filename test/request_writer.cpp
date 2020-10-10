#include <catch2/catch.hpp>

#include <wf700tk/request_writer.hpp>

#include <iostream>

namespace wf700tk {

TEST_CASE("request_writer: common request #1") {
    request_writer w;
    w.msg().inhibit_acceptors_ = true;
    w.msg().tickets_to_dispense(14);
    w.msg().ticket_channel_ = ticket_channel::ticket_3;

    std::vector<unsigned char> buffer(w.length());
    std::vector<unsigned char> result = {0x02, 0x08, 0x10, 0x00,
                                         0x3e, 0x00, 0x03, 0x77};
    REQUIRE(w.write(detail::net::buffer(buffer)));

    CHECK(buffer == result);
}

TEST_CASE("request_writer: poll request") {
    request_writer w;

    std::vector<unsigned char> buffer(w.length());
    std::vector<unsigned char> result = {0x02, 0x08, 0x10, 0x03,
                                         0x00, 0x00, 0x03, 0x77};

    REQUIRE(w.write(detail::net::buffer(buffer)));

    CHECK(buffer == result);
}

} // namespace wf700tk
