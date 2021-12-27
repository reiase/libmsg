#- url parser
add_custom_command(OUTPUT uri.cc
  COMMAND ${RAGEL_EXECUTABLE}
  ARGS
      -G2 -C
      -o ${CMAKE_CURRENT_BINARY_DIR}/uri.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/uri.rl
  DEPENDS extern_ragel uri.rl)
set_source_files_properties(uri.cc PROPERTIES GENERATED TRUE)

add_custom_command(OUTPUT http_header.cc
  COMMAND ${RAGEL_EXECUTABLE}
  ARGS
      -G2 -C
      -o ${CMAKE_CURRENT_BINARY_DIR}/http_header.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/impl/codec/http_header.rl
  DEPENDS extern_ragel impl/codec/http_header.rl)
set_source_files_properties(http_header.cc PROPERTIES GENERATED TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(msg STATIC
  codec.cc
  context.cc
  endpoint.cc
  http_header.cc
  impl/codec/http_codec.hpp
  logging.cc
  uri.cc
  )
target_link_libraries(msg uv)
