#ifndef WF700TK_DETAIL_BASIC_PARSER_HPP
#define WF700TK_DETAIL_BASIC_PARSER_HPP

#include <boost/asio/buffer.hpp>

namespace wf700tk::detail {

namespace net = boost::asio;

struct basic_parser {
    basic_parser(unsigned char msg_length, bool is_request);
    virtual ~basic_parser() = default;

    std::size_t put(const net::const_buffer &buf);

    unsigned int message_length() const;

    bool success() const;

protected:
    virtual bool data_handler(unsigned int pos, char data) = 0;
    virtual void reset_data() = 0;

private:
    const unsigned char *retry(const unsigned char *begin,
                               const unsigned char *absolute_begin);

    enum class state {
        find_start,
        check_head,
        read_data,
        end_byte,
        checksum,
        success
    } state_;

    constexpr const static unsigned char start_byte_ = 0x02;
    constexpr const static unsigned char end_byte_ = 0x03;
    constexpr const static unsigned char request_byte_ = 0x10;
    constexpr const static unsigned char response_byte_ = 0x20;
    constexpr const static unsigned int tail_size_ = 2;

    const std::array<unsigned char, 3> head_;

    unsigned int pos_ = 0;
    const unsigned int msg_length_;
};

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_BASIC_PARSER_HPP
