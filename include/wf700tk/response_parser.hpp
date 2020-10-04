#ifndef WF700TK_DETAIL_RESPONSE_PARSER_HPP
#define WF700TK_DETAIL_RESPONSE_PARSER_HPP

#include <wf700tk/detail/basic_parser.hpp>
#include <wf700tk/response.hpp>

namespace wf700tk {

class response_parser : public detail::basic_parser {
public:
    response_parser();

    const response &message() const;

    response &message();

private:
    bool data_handler(unsigned int pos, char data) override;
    void reset_data() override;

    constexpr const static unsigned char msg_length_ = 0x0b;

    response msg_;
};

} // namespace wf700tk

#endif // WF700TK_DETAIL_RESPONSE_PARSER_HPP
