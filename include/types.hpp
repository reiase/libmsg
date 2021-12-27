#ifndef _LIBMSG_TYPES_HPP_
#define _LIBMSG_TYPES_HPP_

#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>

namespace msg {

struct noncopyable {
  noncopyable() {}
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
};

using Operation = std::function<void(void)>;

struct Frame {
 public:
  enum frame_types {
    MSGFRAME = 1,
    SIGFRAME = 2,
    ASYNCTAG = 3,
    ERRORTAG = 4,
    EMPTYTAG = 5,
  };

 public:
  int64_t type_;
  std::string data_;

 public:
  Frame(int64_t type) : type_(type), data_() {}

  // copy data
  Frame(int64_t t, const char *d, int64_t l) : type_(t), data_(d, l) {}
  Frame(const char *d, int64_t l) : Frame(MSGFRAME, d, l) {}
  Frame(const std::string &str) : Frame(MSGFRAME, str.data(), str.size()) {}

  // take data ownership
  Frame(Frame &&rv) : type_(rv.type_), data_(std::move(rv.data_)) {}
  Frame(int64_t t, Frame &&rv) : type_(t), data_(std::move(rv.data_)) {}

  Frame(int64_t t, std::string &&str) : type_(t), data_(std::move(str)) {}
  Frame(std::string &&str) : Frame(MSGFRAME, std::move(str)) {}

 public:
  static Frame msg() { return Frame(MSGFRAME); }
  static Frame sig() { return Frame(SIGFRAME); }
  static Frame empty() { return Frame(EMPTYTAG); }

 public:
  int64_t size(void) { return data_.size(); }
  char *data(void) { return (char *)data_.data(); }

  bool is_msg() { return type_ == MSGFRAME; }
  bool is_sig() { return type_ == SIGFRAME; }
  bool is_async() { return type_ == ASYNCTAG; }
  bool is_error() { return type_ == ERRORTAG; }
  bool is_empty() { return type_ == EMPTYTAG; }

  Frame as_msg(void) { return Frame(MSGFRAME, std::move(*this)); }
  Frame as_sig(void) { return Frame(SIGFRAME, std::move(*this)); }
  std::string &as_str() { return data_; }

  friend std::ostream &operator<<(std::ostream &os, const Frame &m) {
    return os << m.data_;
  }
};

using WireCallBack = std::function<void(const char *, int64_t)>;
using WireNoCopyCallBack = std::function<void(std::string &&)>;
using MsgHandler = std::function<std::string(std::string &)>;
using SigHandler = std::function<bool(std::string &)>;
using HandlerFactory = std::function<std::pair<MsgHandler, SigHandler>()>;

class OpQueue : public noncopyable {
 private:
  std::list<Operation> queue_;
  std::mutex lock_;

 public:
  void enqueue(Operation &&op) {
    std::lock_guard<std::mutex> lg(lock_);
    queue_.emplace_back(op);
  }

  bool dequeue(Operation &target) {
    {
      std::lock_guard<std::mutex> lg(lock_);
      bool has_op = queue_.size() > 0;
      if (has_op) {
        target = queue_.front();
        queue_.pop_front();
      }
      return has_op;
    }
  }

  std::list<Operation> dequeue_many() {
    std::lock_guard<std::mutex> lg(lock_);
    std::list<Operation> retval;

    queue_.swap(retval);
    return retval;
  }

  bool exec_once() {
    Operation op;
    if (dequeue(op)) {
      op();
      return true;
    } else
      return false;
  }

  size_t exec_many() {
    auto ops = dequeue_many();
    size_t cnt = ops.size();
    while (ops.size() > 0) {
      ops.front()();
      ops.pop_front();
    }
    return ops.size();
  }
};  // end class OpQueue

struct Defer {
  Operation op_;
  Defer(Operation &&op) : op_(op) {}
  ~Defer() { op_(); }
};

using defer = Defer;
}  // namespace msg

#endif  // _LIBMSG_TYPES_HPP_
