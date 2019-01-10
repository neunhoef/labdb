// service.cpp - run the HTTP/2 service

#include "service.h"

#include <sstream>

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

void runService(ServerConfig& config,
                std::size_t num_threads,
                std::shared_ptr<spdlog::logger> log) {

  try {
    boost::system::error_code ec;

    http2 server;

    server.num_threads(num_threads);

    server.handle("/hello", [](const request &req, const response &res) {
      res.write_head(200, {{"x-mini-info", {"hicks"}}});
      VPackBuilder b;
      { VPackObjectBuilder guard(&b);
        b.add("a", VPackValue(1));
        b.add("b", VPackValue("abc"));
        b.add("c", VPackValue(true));
      }
      res.end(b.slice().toJson());
    });

    if (config.encryption) {
      boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
      tls.use_private_key_file(config.keyFile, boost::asio::ssl::context::pem);
      tls.use_certificate_chain_file(config.crtFile);

      configure_tls_context_easy(ec, tls);

      if (server.listen_and_serve(ec, tls, config.bindAddr, config.bindPort)) {
        log->error("Error: {}", ec.message());
      }
    } else {
      if (server.listen_and_serve(ec, config.bindAddr, config.bindPort)) {
        log->error("Error: {}", ec.message());
      }
    }
  } catch (std::exception &e) {
    log->error("Exception: {}", e.what());
  }
}
