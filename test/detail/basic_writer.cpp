#include <catch2/catch.hpp>

#include <wf700tk/detail/basic_writer.hpp>

namespace wf700tk::detail {

struct writer_test : public basic_writer {
    writer_test(unsigned char msg_length, bool is_request,
                unsigned char start_byte = 0x12);

    bool write_data(unsigned char *begin) override;
    unsigned char start_byte_;
    bool result_ = true;
    int call_count_ = 0;
};

writer_test::writer_test(unsigned char msg_length, bool is_request,
                         unsigned char start_byte)
    : basic_writer(msg_length, is_request), start_byte_(start_byte) {}

bool writer_test::write_data(unsigned char *begin) {
    ++call_count_;
    std::generate(begin, begin + length(),
                  [x = start_byte_]() mutable { return x++; });
    return result_;
}

struct data_sequence {
    data_sequence(unsigned char start_byte = 0x12) : pos(start_byte) {}

    using difference_type = std::size_t;
    using value_type = unsigned char;
    using pointer = const unsigned char *;
    using reference = const unsigned char &;
    using iterator_category = std::input_iterator_tag;

    data_sequence &operator++() {
        ++pos;
        return *this;
    }

    data_sequence &operator++(int) { return ++(*this); }

    unsigned char operator*() const { return pos; }
    const unsigned char *operator->() { return &pos; }

    bool operator==(const data_sequence &rhs) const { return pos == rhs.pos; }
    bool operator!=(const data_sequence &rhs) const { return !(*this == rhs); }

    unsigned char pos;
};

void check_head(unsigned char *head, unsigned char len, bool is_request) {
    CHECK(head[0] == 0x02);
    CHECK(head[1] == len);
    CHECK(head[2] == (is_request ? 0x10 : 0x20));
}

void check_tail(unsigned char *begin, unsigned char len) {
    CHECK(begin[len - 2] == 0x03);
    CHECK(begin[len - 1] == 0x77); // TODO: calculate checksum
}

TEST_CASE("basic_writer: check simple request") {
    writer_test w(25, true);
    std::vector<unsigned char> data(25);

    REQUIRE(w.write(net::buffer(data)));
    check_head(data.data(), 25, true);
    CHECK(std::equal(data.begin() + 3, data.end() - 2, data_sequence()));
    check_tail(data.data(), 25);
}

TEST_CASE("basic_writer: check simple response") {
    writer_test w(25, false);
    std::vector<unsigned char> data(25);

    REQUIRE(w.write(net::buffer(data)));
    CHECK(w.call_count_ == 1);
    check_head(data.data(), 25, false);
    CHECK(std::equal(data.begin() + 3, data.end() - 2, data_sequence()));
    check_tail(data.data(), 25);
}

TEST_CASE("basic_writer: buffer to small") {
    writer_test w(25, true);
    std::vector<unsigned char> data(24);

    REQUIRE(w.write(net::buffer(data)) == false);
    CHECK(w.call_count_ == 0);
}

TEST_CASE("basic_writer: error in write_data") {
    writer_test w(25, true);
    w.result_ = false;
    std::vector<unsigned char> data(25);

    REQUIRE(w.write(net::buffer(data)) == false);
    CHECK(w.call_count_ == 1);
}

TEST_CASE("basic_writer: empty response") {
    writer_test w(5, false);
    std::vector<unsigned char> data(5);

    REQUIRE(w.write(net::buffer(data)));
    CHECK(w.call_count_ == 1);
    check_head(data.data(), 5, false);
    check_tail(data.data(), 5);
}

} // namespace wf700tk::detail
