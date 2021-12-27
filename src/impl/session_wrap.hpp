#ifndef _LIBMSG_IMPL_SESSION_WRAP_HPP_
#define _LIBMSG_IMPL_SESSION_WRAP_HPP_
//
#include <memory>

#include "logging.h"
//
#include "codec.hpp"
#include "codec/http_codec.hpp"
#include "types.hpp"
//

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace msg {

template <typename T>
struct SessionWrap {
 public:  // wire callbacks
  WireCallBack recv_;
  WireNoCopyCallBack send_;

 public:  // application handlers
  MsgHandler msg_handler_;
  SigHandler sig_handler_;

 public:  // private data
  T data;
  Wire wire_;
  Codec *codec_;
  std::string buff_;

  SessionWrap(HandlerFactory &make_handler,
              Codec::Factory make_codec = HTTPCodec::create) {
    auto handler_pair = make_handler();
    msg_handler_ = handler_pair.first;
    sig_handler_ = handler_pair.second;

    codec_ = make_codec(wire_);
    recv_ = [this](const char *buf, int64_t len) { this->feed(buf, len); };
    send_ = [this](std::string &&) {};
  }
  ~SessionWrap() { delete codec_; }

  void handle_msg(Frame *frame) {
    Frame rsp = msg_handler_(frame->as_str());      // handle msg
    Frame encoded(std::move(codec_->encode(rsp)));  // encode response
    send_(std::move(encoded.data_));                // send response
  }

  void handle_sig(Frame *sig) {
    if (codec_->handle_signal(sig->as_str())) return;
    sig_handler_(sig->as_str());
  }

  int64_t feed(const char *buf, int64_t len) {
    wire_.feed(buf, len);  // feed data to wire
    while (true) {
      std::string *raw = wire_.read_frame();  // try to get a frame from wire
      if (raw == nullptr) break;              // break if no frame available
      Frame frame(std::move(*raw));  // move buffer ownership to `frame`
      Frame msgsig(std::move(codec_->decode(frame)));  // decode

      if (unlikely(msgsig.is_msg()))
        handle_msg(&msgsig);
      else
        handle_sig(&msgsig);
    }
    return 0L;
  }
};

}  // namespace msg
#endif  // _LIBMSG_IMPL_SESSION_WRAP_HPP_