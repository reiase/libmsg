include(ExternalProject)

ExternalProject_Add(extern_ragel
  BUILD_IN_SOURCE 1
  URL "https://mirrors.aliyun.com/gentoo/distfiles/ragel-6.10.tar.gz"
  URL_MD5 "748cae8b50cffe9efcaa5acebc6abf0d"
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/3rdparty/
  BUILD_COMMAND make -j 4
  INSTALL_COMMAND make install
  )
set(RAGEL_EXECUTABLE ${CMAKE_BINARY_DIR}/3rdparty/bin/ragel)
