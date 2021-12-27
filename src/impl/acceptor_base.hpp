#include "types.hpp"

namespace msg {

template <typename T>
struct AcceptorBase {
  HandlerFactory factory_;

  AcceptorBase(const HandlerFactory &factory) : factory_(factory) {}
  void listen(const std::string &addr) {}
};

}  // namespace msg