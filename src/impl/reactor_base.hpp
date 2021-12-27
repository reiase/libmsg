#include <thread>
//
#include <pthread.h>

//
#include "types.hpp"

namespace msg {

template <typename LOOP>
LOOP &current_thread_event_loop() {
  thread_local LOOP current = nullptr;
  return current;
}

template <typename LOOP>
struct ReactorBase {
  std::thread thread_;
  bool done_;

  OpQueue &queue_;

  ReactorBase(OpQueue &q) : done_(false), queue_(q) {}
  ~ReactorBase() {}

  void trigger(void) {}
  void start(void) { done_ = false; }
  void stop(void) { (done_ = true), thread_.join(); }
  void run(void) {
    return thread_.joinable() ? thread_.join() : []() {}();
  }
};
}  // namespace msg
