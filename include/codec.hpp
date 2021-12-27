#ifndef _LIBMSG_CODEC_HPP_
#define _LIBMSG_CODEC_HPP_

#include <functional>

#include "types.hpp"
#include "wire.hpp"

namespace msg {

class Codec;

class Codec {
 public:
  using Factory = std::function<Codec *(Wire &)>;
  static Codec *create(Wire &wire);

 public:
  Wire &wire_;

 public:
  Codec(Wire &wire) : wire_(wire) {}
  virtual Frame decode(Frame &frame) = 0;  // { return TypedFrame(MSGFRAME); }
  virtual Frame encode(Frame &frame) = 0;  // { return Frame(""); }
  virtual bool handle_signal(const std::string &) = 0;  // { return false; }
};

class DebugCodec : public Codec {
 public:
  static Codec *create(Wire &wire) { return new DebugCodec(wire); }

 public:
  DebugCodec(Wire &wire) : Codec(wire) {}
  virtual Frame decode(Frame &frame) { return Frame::msg(); }
  virtual Frame encode(Frame &frame) { return Frame::empty(); }
  virtual bool handle_signal(const std::string &) { return false; }
};
}  // namespace msg
#endif  // _LIBMSG_CODEC_HPP_
