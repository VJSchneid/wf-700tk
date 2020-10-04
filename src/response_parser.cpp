#include <wf700tk/response_parser.hpp>

namespace wf700tk {

response_parser::response_parser() : detail::basic_parser(msg_length_, false) {}

const response &response_parser::message() const { return msg_; }

response &response_parser::message() { return msg_; }

bool response_parser::data_handler(unsigned int pos, char data) {
    switch (pos) {
    case 0:
        if (data == 0x10) {
            msg_.accepted_credit_ = true;
        } else if (data == 0x01) {
            msg_.accepted_credit_ = false;
        } else {
            return false;
        }
        break;

    case 1:
        msg_.tickets_to_dispense_ = data;
        break;
    case 2:
        if (auto ch = make_credit_channel(data)) {
            msg_.credit_channel_ = *ch;
        } else {
            return false;
        }
        break;
    case 3:
    case 4:
        break;
    case 5:
        if (auto ch = make_ticket_channel(data & 0xf0)) {
            msg_.ticket_channel_ = *ch;
        } else {
            return false;
        }
        msg_.tickets_already_dispensed_ = data & 0x0f;
        break;
    default:
        return false;
    }
    return true;
}

void response_parser::reset_data() {}

} // namespace wf700tk
