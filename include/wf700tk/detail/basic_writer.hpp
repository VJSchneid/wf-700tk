#ifndef WF700TK_DETAIL_BASIC_WRITER_HPP
#define WF700TK_DETAIL_BASIC_WRITER_HPP

#include <vector>

#include <boost/asio/buffer.hpp>

#include <wf700tk/detail/message_base.hpp>

namespace wf700tk::detail {

namespace net = boost::asio;

struct basic_writer : private message_base {
    basic_writer(unsigned int msg_length_, bool is_request);
    virtual ~basic_writer() = default;

    bool write(const net::mutable_buffer &buf);

    using message_base::length;

protected:
    virtual bool write_data(unsigned char *begin) = 0;
};

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_BASIC_WRITER_HPP
