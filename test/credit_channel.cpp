#include <catch2/catch.hpp>

#include <wf700tk/credit_channel.hpp>

namespace wf700tk {

TEST_CASE("valid credit channels are accepted") {
    CHECK(make_credit_channel(0x00) == credit_channel::none);
    CHECK(make_credit_channel(0x08) == credit_channel::input_1);
    CHECK(make_credit_channel(0x10) == credit_channel::input_2);
    CHECK(make_credit_channel(0x18) == credit_channel::input_3);
    CHECK(make_credit_channel(0x20) == credit_channel::input_4);
}

TEST_CASE("invalid credit channels are rejected") {
    CHECK(!make_credit_channel(0x01));
    CHECK(!make_credit_channel(0x11));
    CHECK(!make_credit_channel(0x28));
    CHECK(!make_credit_channel(0x17));
    CHECK(!make_credit_channel(0x19));
}

} // namespace wf700tk
