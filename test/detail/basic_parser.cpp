#include <catch2/catch.hpp>

#include <array>
#include <set>

#include <wf700tk/detail/basic_parser.hpp>

namespace wf700tk::detail {

struct parser_test : basic_parser {
    parser_test(unsigned char msg_length, bool is_request)
        : basic_parser(msg_length, is_request) {
        reset_data();
    }

    bool data_handler(unsigned int pos, char data) override {
        REQUIRE(!data_[pos]);
        data_[pos] = data;
        return valid_length_-- != 0;
    }

    void reset_data() override {
        data_.clear();
        if (length() >= 5) {
            data_.resize(length() - 5);
            valid_length_ = length() - 5;
        }
    }

    bool data_complete() const {
        return data_.end() ==
               std::find(data_.begin(), data_.end(), std::nullopt);
    }

    int valid_length_ = -1;
    std::vector<std::optional<unsigned char>> data_;
};

template <typename Container>
void validate_request(const parser_test &p, const Container &result) {
    REQUIRE(p.success());
    REQUIRE(p.data_complete());
    REQUIRE(p.valid_length_ == 0);
    CHECK(std::equal(p.data_.begin(), p.data_.end(), result.begin() + 3));
}

constexpr const std::array<unsigned char, 8> test_response_common = {
    0x02, 0x08, 0x10, 0x03, 0x15, 0x00, 0x03, 0x77};

constexpr const std::array<unsigned char, 11> test_request_common = {
    0x02, 0x0b, 0x20, 0x01, 0x0e, 0x14, 0x15, 0x16, 0x21, 0x03, 0x77};

constexpr const std::array<unsigned char, 5> test_request_minimal = {
    0x02, 0x05, 0x20, 0x03, 0x77};

std::vector<unsigned char> double_message() {
    std::vector<unsigned char> buffer(test_request_common.size() * 2);
    std::copy(test_request_common.begin(), test_request_common.end(),
              buffer.begin());
    std::copy(test_request_common.begin(), test_request_common.end(),
              buffer.begin() + test_request_common.size());

    buffer[3] = 0xa1;
    buffer[4] = 0xa2;
    buffer[5] = 0xa3;
    buffer[6] = 0xa4;
    buffer[7] = 0xa5;
    buffer[8] = 0xa6;
    return buffer;
}

TEST_CASE("parser accepts correct message") {
    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(test_request_common)) ==
            test_request_common.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser finds message with some leading garbage") {
    parser_test p(11, false);
    std::vector<unsigned char> msg(test_request_common.size() + 10);
    std::generate(msg.begin(), msg.begin() + 10, std::mt19937());
    std::copy(test_request_common.begin(), test_request_common.end(),
              msg.begin() + 10);

    REQUIRE(p.put(net::buffer(msg)) == msg.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser accepts message without data") {
    parser_test p(5, false);
    REQUIRE(p.put(net::buffer(test_request_minimal)) ==
            test_request_minimal.size());
    validate_request(p, test_request_minimal);
}

TEST_CASE("parser accepts response") {
    parser_test p(8, true);
    REQUIRE(p.put(net::buffer(test_response_common)) ==
            test_response_common.size());
    validate_request(p, test_response_common);
}

TEST_CASE("parser error detection: in start byte") {
    auto msg = double_message();
    msg[0] = 0xff;

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg)) == msg.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser error detection: in length") {
    auto msg = double_message();
    msg[1] = 0xff;

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg)) == msg.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser error detection: in data") {
    auto msg = double_message();

    parser_test p(11, false);
    p.valid_length_ = 0;
    REQUIRE(p.put(net::buffer(msg)) == msg.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser error detection: in end byte") {
    auto msg = double_message();
    msg[9] = 0xff;

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg)) == msg.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser gets garbage message") {
    std::vector<unsigned char> msg(30);
    std::generate(msg.begin(), msg.end(), std::mt19937());

    parser_test p(0, false);
    REQUIRE(p.put(net::buffer(msg)) == msg.size());
}

TEST_CASE("parse splitted message with leading garbage") {
    std::vector<unsigned char> msg(30);
    std::generate(msg.begin(), msg.end(), std::mt19937());

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg)) == msg.size());

    REQUIRE(p.put(net::buffer(test_request_common)) ==
            test_request_common.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser reads not to much") {
    std::vector<unsigned char> msg(test_request_common.size() + 1);
    std::copy(test_request_common.begin(), test_request_common.end(),
              msg.begin());

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg)) == test_request_common.size());
    validate_request(p, test_request_common);
}

TEST_CASE("parser reads splitted message in head") {
    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(test_request_common.data(), 2)) == 2);
    REQUIRE(p.put(net::buffer(test_request_common.data() + 2, 9)) == 9);
    validate_request(p, test_request_common);
}

TEST_CASE("parser reads splitted message in data") {
    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(test_request_common.data(), 5)) == 5);
    REQUIRE(p.put(net::buffer(test_request_common.data() + 5, 6)) == 6);
    validate_request(p, test_request_common);
}

TEST_CASE("parser reads two messages") {
    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(test_request_common)) == 11);
    validate_request(p, test_request_common);
    REQUIRE(p.put(net::buffer(test_request_common)) == 11);
    validate_request(p, test_request_common);
}

TEST_CASE("parser handles ack") {
    parser_test p(11, false);
    auto request = test_request_common;
    request[2] |= 0b1111;
    REQUIRE(p.put(net::buffer(request)) == 11);
}

TEST_CASE(
    "parser reads incorrect message but can not jump to absolute beginning") {
    auto msg = double_message();
    msg[9] = 0xff;

    parser_test p(11, false);
    REQUIRE(p.put(net::buffer(msg.data(), 6)) == 6);
    REQUIRE(p.put(net::buffer(msg.data() + 6, msg.size() - 6)) ==
            msg.size() - 6);
    validate_request(p, test_request_common);
}

} // namespace wf700tk::detail
