enable_testing()

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/_deps/libcatch2-src/contrib)
INCLUDE(Catch)

include_directories(../src)
include_directories(${CMAKE_BINARY_DIR}/_deps/libcatch2-src/single_include)

add_executable(test_logging
test_logging.cc
)
target_link_libraries(test_logging PRIVATE msg Catch2::Catch2WithMain)
catch_discover_tests(test_logging)

add_executable(test_frame
test_frame.cc
)
target_link_libraries(test_frame PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(test_frame)

add_executable(test_context
  test_context.cc
)
target_link_libraries(test_context PRIVATE msg Catch2::Catch2WithMain)
catch_discover_tests(test_context)

add_executable(test_server
test_server.cc
)
target_link_libraries(test_server PRIVATE msg Catch2::Catch2WithMain)
catch_discover_tests(test_server)

add_executable(notest_server
notest_server.cc
)
target_link_libraries(notest_server PRIVATE msg)
