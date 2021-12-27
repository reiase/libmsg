#include "logging.h"

namespace msg {
int& loglevel() {
  static int value = LOG_INFO;
  return value;
}
}  // namespace msg

extern "C" {
int current_loglevel() { return msg::loglevel(); }
void set_current_loglevel(int log_level) { msg::loglevel() = log_level; }
}