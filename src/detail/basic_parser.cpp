#include <wf700tk/detail/basic_parser.hpp>

#include <algorithm>

namespace wf700tk::detail {

basic_parser::basic_parser(unsigned char msg_length, bool is_request)
    : message_base(msg_length, is_request) {}

std::size_t basic_parser::put(const net::const_buffer &buf) {
    auto begin = net::buffer_cast<const unsigned char *>(buf);
    const auto absolute_begin = begin;
    auto end = begin + net::buffer_size(buf);

    while (begin != end) {
        switch (state_) {
        case state::find_start: {
            begin = std::find(begin, end, head_[0]);
            if (begin == end) {
                return net::buffer_size(buf);
            }
            ++begin;
            pos_ = 1;
            state_ = state::check_head;
            break;
        }

        case state::check_head: {
            bool partial = false;
            auto head_end = begin + head_.size() - pos_;

            if (head_end > end) {
                partial = true;
                head_end = end;
            }

            if (std::equal(begin, head_end, head_.begin() + pos_)) {
                pos_ += std::distance(begin, head_end);
                begin = head_end;
                if (!partial) {
                    state_ = state::read_data;
                }
            } else {
                state_ = state::find_start;
            }
            break;
        }

        case state::read_data: {
            unsigned int msg_len = length();
            for (; begin != end; ++begin, ++pos_) {
                if (pos_ == msg_len - tail_size_) {
                    state_ = state::end_byte;
                    break;
                }

                if (!data_handler(pos_ - head_.size(), *begin)) {
                    reset_data();
                    begin = retry(begin, absolute_begin);
                    break;
                }
            }
            break;
        }

        case state::end_byte:
            if (*begin != end_byte_) {
                reset_data();
                begin = retry(begin, absolute_begin);
                break;
            }
            ++begin;
            state_ = state::checksum;
            break;

        case state::checksum:
            // TODO: validate checksum
            ++begin;
            state_ = state::success;
            return std::distance(absolute_begin, begin);
        case state::success:
            reset_data();
            state_ = state::find_start;
            break;
        }
    }
    return net::buffer_size(buf);
}

bool basic_parser::success() const { return state_ == state::success; }

const unsigned char *basic_parser::retry(const unsigned char *begin,
                                         const unsigned char *absolute_begin) {
    state_ = state::find_start;
    if (std::distance(absolute_begin, begin) < static_cast<long>(pos_)) {
        return absolute_begin + 1;
    } else {
        return begin - pos_ + 1;
    }
}

} // namespace wf700tk::detail
