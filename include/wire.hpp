#ifndef _LIBMSG_WIRE_HPP_
#define _LIBMSG_WIRE_HPP_

#include <cstdint>
#include <string>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace msg {

namespace {
static const uint16_t CRLF = 0x0d0a;
inline bool is_crlf(uint16_t &ending, uint8_t c) {
  ending = (ending << 8) + c;
  return ending == CRLF;
}
}  // namespace

struct Wire {  // struct wire for keeping io buffers
  const char *buffer_;
  int32_t offset_;
  int32_t length_;
  int32_t token_size_;
  uint16_t last_char_;
  std::string iframe_;

  Wire() : buffer_(NULL), offset_(0), length_(0) {}
  inline void feed(const char *data, std::size_t size) {
    buffer_ = data;
    length_ = size;
    offset_ = 0;
  }
  inline void clear_frame() { iframe_.clear(); }
  inline void read_size(int32_t size) {
    token_size_ = size;
    if (size >= 0)
      iframe_.reserve(size);
    else
      iframe_.reserve(512);
  }

  // read some data
  inline int32_t read_upto(std::string &buff, int32_t size) {
    int32_t rsize = std::min(length_ - offset_, size);
    buff.append(buffer_ + offset_, rsize);
    offset_ += rsize;
    return rsize;
  }

  // read until CRLF
  inline int32_t read_crlf(std::string &buff, uint16_t &ending) {
    int32_t i = 0;
    if (unlikely(offset_ >= length_)) return 0;
    for (i = 0;                                           //
         likely(i < length_ - offset_ &&                  //
                (!is_crlf(ending, buffer_[offset_ + i]))  //
         );                                               //
         i++)                                             //
      ;
    buff.append(buffer_ + offset_, i + 1);
    offset_ += (i + 1);
    return i + 1;
  }

  inline std::string *read_frame() {
    int32_t retval = 0;
    if (unlikely(token_size_ >= 0)) {
      retval = read_upto(iframe_, token_size_ - iframe_.size());
      if (iframe_.size() < token_size_) return NULL;
    } else {
      retval = read_crlf(iframe_, last_char_);
      if (retval == 0 || last_char_ != CRLF) return NULL;
    }
    last_char_ = 0;
    return &iframe_;
  }
};

}  // namespace msg
#endif  // ifndef _LIBMSG_WIRE_HPP_