#ifndef WF700TK_DETAIL_RESPONSE_PARSER_HPP
#define WF700TK_DETAIL_RESPONSE_PARSER_HPP

#include <wf700tk/detail/basic_parser.hpp>
#include <wf700tk/response.hpp>

namespace wf700tk::detail {

class response_parser : public basic_parser {
    response_parser();

private:
    virtual bool data_handler(unsigned int pos, char data);

    constexpr const static unsigned char msg_length_ = 0x0b;

    response msg_;
};

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_RESPONSE_PARSER_HPP
