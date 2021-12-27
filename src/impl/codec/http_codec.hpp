// #define LOGLEVEL LOGLEVEL_INFO
#include <map>
//
#include "codec.hpp"
#include "logging.h"
#include "types.hpp"
#include "utils.hpp"

namespace msg {

struct HTTPContext {
  std::string method_;
  std::string uri_;
  std::string proto_;
  int32_t code_;

  std::map<std::string, std::string> values;

  void reset() { values.clear(); }
};

class HTTPCodec;
bool parse_header(const std::string &header, HTTPCodec &codec_);

class HTTPCodec : public Codec {
 public:
  static Codec *create(Wire &wire) { return new HTTPCodec(wire); }
  enum STATUS {
    HTTP_BEGIN,
    HTTP_HEAD,
    HTTP_BODY,
  };
  enum FLAGS {
    UNIT = 1,
    KEEP_ALIVE = UNIT << 1,
    CONTENT_LENGTH = UNIT << 2,
  };

  HTTPContext ctx_;
  STATUS status_;
  int64_t flags_;
  int64_t content_length_;

 public:
  HTTPCodec(Wire &wire)
      : Codec(wire), status_(HTTP_BEGIN), content_length_(-1) {
    wire.read_size(-1);
  }
  Frame decode(Frame &frame) {
    switch (status_) {
      case HTTP_BEGIN:
        status_ = HTTP_HEAD;
        wire_.read_size(-1);
        return frame.as_sig();

      case HTTP_HEAD:
        if (frame.as_str() == "\r\n") {
          status_ = HTTP_BODY;
          wire_.read_size(content_length_);

          return Frame::empty();
        } else {
          return frame.as_sig();
        }
      case HTTP_BODY:
        status_ = HTTP_BEGIN;
        wire_.read_size(-1);

        return frame.as_msg();
      default:
        return Frame::empty();
    }
  }
  virtual Frame encode(Frame &frame) {
    std::string msg;
    msg.reserve(1024);
    if (ctx_.code_ == 0) ctx_.code_ = 200;

    msg += ctx_.proto_;
    msg += " ";
    msg += ToString(ctx_.code_);
    msg += "\r\nContent-Length: ";
    msg += ToString(frame.size());
    msg += "\r\nConnection: Keep-Alive\r\n\r\n";
    msg.append(frame.data(), frame.size());
    return Frame(std::move(msg));
  }

  bool handle_signal(const std::string &sig) {
    return parse_header(sig, *this);
  }
  void handle_header(std::string name, std::string value);
};

}  // namespace msg
