#include <thread>
//
#include <pthread.h>
//
#include <uv.h>

#include "impl/reactor_base.hpp"

namespace msg {

struct UVReactor : public ReactorBase<UVReactor> {
  uv_loop_t loop_;
  uv_async_t trigger_;

  UVReactor(OpQueue &q) : ReactorBase<UVReactor>(q) {
    uv_loop_init(&loop_);
    uv_async_init(&loop_, &trigger_, UVReactor::trigger_cb);
    trigger_.data = (void *)this;
  }
  ~UVReactor() { uv_loop_close(&loop_); }

  void trigger(void) { uv_async_send(&trigger_); }
  void start() {
    done_ = false;
    thread_ = std::move(std::thread([this]() {
      // while (!done_) {
      current_thread_event_loop<uv_loop_t *>() = &loop_;
      uv_run(&loop_, UV_RUN_DEFAULT);
      current_thread_event_loop<uv_loop_t *>() = nullptr;
      // }
    }));
  }
  void stop(void) {
    done_ = true;
    trigger();
    thread_.join();
  }

  static inline void trigger_cb(uv_async_t *handle);
};

void UVReactor::trigger_cb(uv_async_t *handle) {
  UVReactor *self = (UVReactor *)handle->data;
  if (self->done_)
    uv_stop(&(self->loop_));
  else
    self->queue_.exec_many();
}

}  // namespace msg
