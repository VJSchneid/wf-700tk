#ifndef WF700TK_DETAIL_MESSAGE_BASE_HPP
#define WF700TK_DETAIL_MESSAGE_BASE_HPP

#include <array>

namespace wf700tk::detail {

struct message_base {
    message_base(unsigned char length, bool is_request);

    constexpr const static unsigned char start_byte_ = 0x02;
    constexpr const static unsigned char end_byte_ = 0x03;
    constexpr const static unsigned char request_byte_ = 0x10;
    constexpr const static unsigned char response_byte_ = 0x20;
    constexpr const static unsigned int tail_size_ = 2;

    unsigned char length() const;

    bool ack(unsigned char v);
    unsigned char ack() const;

    std::array<unsigned char, 3> head_;
};

#if false

enum class msg_type : unsigned char {
    master_to_wf700tk = 0x10,
    wf700tk_to_master = 0x20
};

/**
 * @brief The msg_head struct contains all
 *        information for the starting sequence
 *        with exceptions to the length
 */
struct msg_head {
public:
    constexpr msg_type type() const noexcept { return type_; }
    constexpr void type(msg_type v) noexcept { type_ = v; }

    /// @returns false if v is invalid, i.e. exceeds 0b111
    constexpr bool type(unsigned char v) noexcept {
        if (v > 0b111) {
            return false;
        }
        type_ = static_cast<msg_type>(v << 4);
        return true;
    }

    constexpr unsigned char ack() const noexcept { return ack_; }

    /// @returns false if v is invalid, i.e. exceeds 0xf
    constexpr bool ack(unsigned char v) noexcept {
        if (v > 0x0f) {
            return false;
        }
        ack_ = v;
        return true;
    }

private:
    msg_type type_ = msg_type::master_to_wf700tk;
    unsigned char ack_ = 0x00;
};

/**
 * @brief WF-700TK message holding generic data content
 *
 * This class holds the message head information and customizable data
 * fields for the message.
 *
 * @tparam DataType The type used to generate the message's data
 * @param[in] isRequest \a true if message is send from master to WF-700TK
 *                      otherwise \a false
 */
template <typename Data>
struct message : public msg_head, private Data::value_type {
    using data_type = typename Data::value_type;

    constexpr message() noexcept : data_type() { Data::set_defaults(*this); }

    constexpr const data_type &data() const noexcept {
        return static_cast<const data_type &>(*this);
    }
    constexpr data_type &data() noexcept {
        return static_cast<data_type &>(*this);
    }

    constexpr unsigned char length() const noexcept {
        return Data::data_length(*this) + 5;
    }
};

struct null_data {
    struct value_type {
        unsigned length = 0;
    };

    constexpr static void set_defaults(message<null_data> &v) noexcept {
        v.type(msg_type::master_to_wf700tk);
    }

    constexpr static unsigned char data_length(message<null_data> &v) {
        return v.data().length;
    }
};

#endif

} // namespace wf700tk::detail

#endif // WF700TK_DETAIL_MESSAGE_BASE_HPP
