#include "context.hpp"

#include <atomic>
#include <vector>

#include "impl/uv/uvreactor.hpp"

namespace msg {

namespace {

struct Executor : public UVReactor, noncopyable {
  OpQueue queue_;
  std::thread thread_;
  bool done_;

  Executor() : UVReactor(queue_) { this->start(); }
  Executor(Executor &&other) : Executor() {}
  ~Executor() { this->stop(); }

  inline void schedule(Operation &&op, bool trigger = false) {
    queue_.enqueue(std::move(op));
    if (trigger) this->trigger();
  }
  inline void submit(Operation &&op) { schedule(std::move(op), true); }
};  // namespace

}  // namespace

struct Context::Impl {
  std::vector<Executor> executors_;
  std::atomic<int64_t> counter_;

  Impl(int64_t nthreads) : counter_(0L) {
    for (int i = 0; i < nthreads; i++) executors_.emplace_back();
  }

  void run() {
    for (auto &exc : executors_) exc.run();
  }
  void stop() {
    for (auto &exc : executors_) exc.stop();
  }

  inline Executor &select(int64_t idx = -1) {
    Executor &retval =
        idx >= 0 ? executors_[idx]
                 : executors_[counter_.load(std::memory_order_relaxed) %
                              executors_.size()];
    counter_.fetch_add(1L, std::memory_order_relaxed);
    return retval;
  }
  inline void schedule(Operation &&op, int64_t idx = -1, bool trigger = false) {
    return trigger ? select(idx).submit(std::move(op))
                   : select(idx).schedule(std::move(op));
  }
  inline void submit(Operation &&op) { schedule(std::move(op), -1, true); }
};

/// class Context

Context::Context(int64_t nthread) { impl_ = new Context::Impl(nthread); }
Context::~Context() { delete impl_; }

int64_t Context::size() const { return impl_->executors_.size(); }

void Context::append(Operation &&op) const { impl_->schedule(std::move(op)); }
void Context::submit(Operation &&op) const { impl_->submit(std::move(op)); }
void Context::append(int64_t idx, Operation &&op) const {
  impl_->schedule(std::move(op), idx, false);
}
void Context::submit(int64_t idx, Operation &&op) const {
  impl_->schedule(std::move(op), idx, true);
}

void Context::run() const { impl_->run(); }
void Context::stop() const { impl_->stop(); }

}  // namespace msg
