#ifndef _LIBMSG_CONTEXT_H_
#define _LIBMSG_CONTEXT_H_

#include <string>

#include "types.hpp"

namespace msg {

class Context : public noncopyable {
 public:
  Context(int64_t nthread);
  ~Context();

  int64_t size() const;

  void append(Operation &&op) const;
  void submit(Operation &&op) const;

  void append(int64_t idx, Operation &&op) const;
  void submit(int64_t idx, Operation &&op) const;

  Operation scope_submit() {
    return [this]() { this->submit(std::move([]() {})); };
  }

  void run() const;
  void stop() const;

 private:
  struct Impl;
  Impl *impl_;
};

};  // namespace msg

#endif  // _LIBMSG_CONTEXT_H_
