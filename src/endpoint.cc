#include "endpoint.hpp"

#include "codec.hpp"
#include "impl/uv/uvacceptor.hpp"

namespace msg {

struct EndPoint::Impl {
 public:
  const Context &ctx_;
  std::string addr_;
  HandlerFactory factory_;

  Impl(const Context &ctx) : ctx_(ctx) {}

 public:
  void bind(const std::string &addr) {
    addr_ = addr;
    for (int64_t i = 0; i < ctx_.size(); i++)
      ctx_.submit(i, [this]() {
        UVAcceptor *acceptor = new UVAcceptor(factory_);
        acceptor->listen(addr_);
      });
  }
};

EndPoint::EndPoint(const Context &ctx) : impl_(new EndPoint::Impl(ctx)) {}
EndPoint::~EndPoint() {}

void EndPoint::init(HandlerFactory factory) { impl_->factory_ = factory; }

void EndPoint::bind(const std::string &addr) { impl_->bind(addr); }

}  // namespace msg