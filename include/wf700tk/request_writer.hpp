#ifndef WF700TK_REQUEST_WRITER_HPP
#define WF700TK_REQUEST_WRITER_HPP

#include <wf700tk/detail/basic_writer.hpp>
#include <wf700tk/request.hpp>

namespace wf700tk {

struct request_writer : public detail::basic_writer {
    request_writer();

    inline const request &msg() const { return msg_; }
    inline request &msg() { return msg_; }

private:
    bool write_data(unsigned char *data) override;

    request msg_;
};

} // namespace wf700tk

#endif // WF700TK_REQUEST_WRITER_HPP
