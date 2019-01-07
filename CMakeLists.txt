cmake_minimum_required(VERSION 3.10)
project(protodb VERSION 0.0.1)

find_package(OpenSSL 1.1.0)
find_package(Boost 1.65.0 REQUIRED system thread)

add_subdirectory(3rdParty)

add_executable(labdb
  src/labdb.cpp
)

target_include_directories(labdb PRIVATE
  ${OPENSSL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${nghttp2_SOURCE_DIR}/src/includes
  3rdParty/docopt.cpp
)

target_link_libraries(labdb
  docopt
  nghttp2
  nghttp2_asio
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)

# Other, experimental stuff:

add_executable(asio-sv asio-sv.cpp)
add_executable(asio-cl asio-cl.cpp)
add_executable(mini mini.cpp)

target_include_directories(asio-sv PRIVATE
  ${OPENSSL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${nghttp2_SOURCE_DIR}/src/includes
)

target_link_libraries(asio-sv
  nghttp2
  nghttp2_asio
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)

target_include_directories(asio-cl PRIVATE
  ${OPENSSL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${nghttp2_SOURCE_DIR}/src/includes
)

target_link_libraries(asio-cl
  nghttp2
  nghttp2_asio
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)

target_include_directories(mini PRIVATE
  ${OPENSSL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${nghttp2_SOURCE_DIR}/src/includes
)

target_link_libraries(mini
  nghttp2
  nghttp2_asio
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)

