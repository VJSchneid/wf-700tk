#ifndef WF700TK_DETAIL_BASIC_PARSER_HPP
#define WF700TK_DETAIL_BASIC_PARSER_HPP

#include <boost/asio/buffer.hpp>

#include <wf700tk/detail/message_base.hpp>

namespace wf700tk::detail {

namespace net = boost::asio;

struct basic_parser : private message_base {
    basic_parser(unsigned char msg_length, bool is_request);
    virtual ~basic_parser() = default;

    std::size_t put(const net::const_buffer &buf);

    using message_base::length;

    bool success() const;

protected:
    virtual bool data_handler(unsigned int pos, char data) = 0;
    virtual void reset_data() = 0;

private:
    const unsigned char *retry(const unsigned char *begin,
                               const unsigned char *absolute_begin);

    enum class state {
        find_start,
        check_length,
        check_msg_type_and_ack,
        read_data,
        end_byte,
        checksum,
        success
    } state_ = state::find_start;

    unsigned int pos_ = 0;
};

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_BASIC_PARSER_HPP
