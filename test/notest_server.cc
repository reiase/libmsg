#include <pthread.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
//
#include "endpoint.hpp"
#include "logging.h"
#include "types.hpp"

int main() {
  msg::Context ctx1(12);
  msg::EndPoint ep(ctx1);

  ep.init([]() {
    return std::make_pair(  //
        [](std::string& msg) {
          // std::cout << msg.as_str() << std::endl;
          return msg;
        },  //
        [](std::string& sig) { return true; });
  });
  ep.bind("0.0.0.0:8888");
  system("sleep 1000");
}
