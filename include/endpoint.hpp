#ifndef _LIBMSG_ENDPOINT_H_
#define _LIBMSG_ENDPOINT_H_

#include <string>

#include "context.hpp"
#include "types.hpp"

namespace msg {
class EndPoint : public noncopyable {
 public:
  EndPoint(const Context &ctx);
  ~EndPoint();

  void init(HandlerFactory factory);

  void bind(const std::string &addr);
  void conn(const std::string &addr);

 private:
  struct Impl;
  Impl *impl_;
};

class MessageQueue : public noncopyable {
 public:
  MessageQueue(EndPoint &endpoint);
  MessageQueue(EndPoint &endpoint, size_t max_size);
};

};  // namespace msg

#endif  // _LIBMSG_ENDPOINT_H_
