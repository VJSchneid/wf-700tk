#include <wf700tk/detail/message_base.hpp>

namespace wf700tk::detail {

message_base::message_base(unsigned char length, bool is_request)
    : head_(
          {start_byte_, length, is_request ? request_byte_ : response_byte_}) {}

unsigned char message_base::length() const { return head_[1]; }

} // namespace wf700tk::detail
