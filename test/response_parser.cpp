#include <catch2/catch.hpp>

#include <wf700tk/response_parser.hpp>

namespace wf700tk {

struct response_data {
    std::array<unsigned char, 11> data;
    response result;

    detail::net::const_buffer buffer() const {
        return detail::net::buffer(data);
    }
};

static void check_response(const response &lhs, const response &rhs) {
    CHECK(lhs.accepted_credit_ == rhs.accepted_credit_);
    CHECK(lhs.credit_channel_ == rhs.credit_channel_);
    CHECK(lhs.ticket_channel_ == rhs.ticket_channel_);
    CHECK(lhs.tickets_already_dispensed_ == rhs.tickets_already_dispensed_);
    CHECK(lhs.tickets_to_dispense_ == rhs.tickets_to_dispense_);
}

const static response_data response1{
    {0x02, 0x0b, 0x20, 0x01, 0x0e, 0x00, 0x00, 0x00, 0x21, 0x03, 0x15},
    response(false, credit_channel::none, ticket_channel::ticket_2, 14, 1)};

const static response_data response2{
    {0x02, 0x0b, 0x20, 0x10, 0x02, 0x18, 0x00, 0x00, 0x4f, 0x03, 0x77},
    response(true, credit_channel::input_3, ticket_channel::ticket_4, 2, 15)};

const static response_data response3{
    {0x02, 0x0b, 0x20, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x03, 0x77},
    response(true, credit_channel::input_4, ticket_channel::none, 0, 0)};

std::vector<unsigned char> modify_response1(unsigned int byte_no,
                                            unsigned char value) {
    std::vector<unsigned char> data(response1.data.begin(),
                                    response1.data.end());
    data[byte_no] = value;
    return data;
}

TEST_CASE("response_parser: ticket dispense update") {
    response_parser p;
    p.put(response1.buffer());
    REQUIRE(p.success());
    check_response(p.message(), response1.result);
}

TEST_CASE("response_parser: ticket dispense update and coin update") {
    response_parser p;
    p.put(response2.buffer());
    REQUIRE(p.success());
    check_response(p.message(), response2.result);
}

TEST_CASE("response_parser: only coin update") {
    response_parser p;
    p.put(response3.buffer());
    REQUIRE(p.success());
    check_response(p.message(), response3.result);
}

TEST_CASE("response_parser: invalid credit channel") {
    response_parser p;
    auto data = modify_response1(5, 0x1a);

    p.put(detail::net::buffer(data));
    CHECK(!p.success());
}

TEST_CASE("response_parser: invalid status byte") {
    response_parser p;
    auto data = modify_response1(3, 0x12);

    p.put(detail::net::buffer(data));
    CHECK(!p.success());
}

TEST_CASE("response_parser: invalid ticket channel") {
    response_parser p;
    auto data = modify_response1(8, 0x60);

    p.put(detail::net::buffer(data));
    CHECK(!p.success());
}

} // namespace wf700tk
