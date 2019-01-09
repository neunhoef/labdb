#include <iostream>
#include <sstream>
#include <string>

#include <docopt.h>
#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

#include "uuid.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

static const char USAGE[] =
R"(LabDB.

  Usage:
    labdb [-h] [--version] [-b BIND] [-p PORT] [-t THR] [-a ADDR] [-c CERT]
          [-k KEY] [-d DBDIR] [-l LOG] [-v]

  Options:
    -h --help               Only show this screen.
    --version               Only show the version.
    -a ADDR --address=ADDR  Advertised address as URL.
    -b BIND --bind=BIND     Bind address for server [default: 0.0.0.0].
    -c CERT --cert=CERT     File name of certificate chain file (PEM) [default: certs/server.chain.pem].
    -d DBDIR --dbdir=DBDIR  Database directory [default: data].
    -k KEY --key=KEY        File name of private key file (PEM) [default: certs/server.key].
    -l LOG --log=LOG        File name for the log [default: labdb.log].
    -p PORT --port=PORT     Bind port for server [default: 9000].
    -t THR --threads=THR    Number of threads [default: 1].
    -v --verbose            Verbose logging

)";

#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

int main(int argc, char *argv[]) {
  std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,               // show help if requested
                         "LabDB 0.1");       // version string

  // Logging:
  std::string logfile = args["--log"].asString();
  // If this throws, we just crash:
  auto logger = spdlog::basic_logger_mt("logger", logfile);
  logger->info("Welcome to LabDB!");

  // Database directory:
  std::string dbdir = args["--dbdir"].asString();
  mkdir(dbdir.c_str(), 0755);  // ignore error if it exists
  
  // UUID:
  std::string theUUID = readOrMakeUUID(dbdir);
  std::cout << "My UUID is " << theUUID << std::endl;

  // Show command line arguments in log if verbose:
  if (args["--verbose"].asBool()) {
    logger->info("Command line arguments:");
    for (auto const& p : args) {
      std::stringstream s;
      s << "Attribute: " << p.first << " Value: " << p.second;
      logger->info(s.str());
    }
  }

  logger->flush();

  try {
    boost::system::error_code ec;

    std::string addr = args["--bind"].asString();
    std::string port = args["--port"].asString();
    std::size_t num_threads = args["--threads"].asLong();

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

    std::string key = args["--key"].asString();
    std::string cert = args["--cert"].asString();
    if (!key.empty() && !cert.empty()) {
      boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);
      tls.use_private_key_file(key, boost::asio::ssl::context::pem);
      tls.use_certificate_chain_file(cert);

      configure_tls_context_easy(ec, tls);

      if (server.listen_and_serve(ec, tls, addr, port)) {
        std::cerr << "error: " << ec.message() << std::endl;
      }
    } else {
      if (server.listen_and_serve(ec, addr, port)) {
        std::cerr << "error: " << ec.message() << std::endl;
      }
    }
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
