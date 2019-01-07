#include <iostream>
#include <string>

#include <docopt.h>
#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

static const char USAGE[] =
R"(LabDB.

  Usage:
    labdb [-h] [--version] [-b BIND] [-p PORT] [-t THR] [-a ADDR] [-c CERT]
          [-k KEY]

  Options:
    -h --help               Only show this screen.
    --version               Only show the version.
    -a ADDR --address=ADDR  Advertised address as URL.
    -b BIND --bind=BIND     Bind address for server [default: 0.0.0.0].
    -c CERT --cert=CERT     File name of certificate chain file (PEM) [default: certs/server.chain.pem].
    -k KEY --key=KEY        File name of private key file (PEM) [default: certs/server.key].
    -p PORT --port=PORT     Bind port for server [default: 9000].
    -t THR --threads=THR    Number of threads [default: 1].

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
  for (auto const& p : args) {
    std::cout << "Attribute:" << p.first << " Value:" << p.second << std::endl;
  }
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
