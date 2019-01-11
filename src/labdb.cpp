// labdb.cpp - main file

#include <iostream>
#include <string>
#include <sstream>

#include <docopt.h>

#include "uuid.h"
#include "service.h"
#include "serverconfig.h"
#include "peers.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

static const char USAGE[] =
R"(LabDB.

  Usage:
    labdb [-h] [--version] [-b BIND] [-p PUBADDR] [-t THR] [-a ADDR] [-c CERT]
          [-k KEY] [-d DBDIR] [-l LOG] [-v]

  Options:
    -h --help                 Only show this screen.
    --version                 Only show the version.
    -a ADDR --address=ADDR    Advertised address as URL.
    -b BIND --bind=BIND       Bind address for server [default: https://0.0.0.0:9000].
    -c CERT --cert=CERT       File name of certificate chain file (PEM) [default: certs/server.chain.pem].
    -d DBDIR --dbdir=DBDIR    Database directory [default: data].
    -j LEADER --join=LEADER   Leader address to join cluster.
    -k KEY --key=KEY          File name of private key file (PEM) [default: certs/server.key].
    -l LOG --log=LOG          File name for the log [default: -].
    -p PUBADDR --pub=PUBADDR  Bind port for server.
    -t THR --threads=THR      Number of threads [default: 1].
    -v --verbose              Verbose logging

)";

int main(int argc, char *argv[]) {
  std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,               // show help if requested
                         "LabDB 0.1");       // version string

  // Logging:
  std::shared_ptr<spdlog::logger> log;
  try {
    std::string logfile = args["--log"].asString();
    if (logfile == "-") {
      log = spdlog::stdout_color_mt("log");
    } else {
      log = spdlog::basic_logger_mt("log", logfile);
    }
  }
  catch (...) {
    std::cerr << "Cannot initialize logger, giving up." << std::endl;
    _exit(1);
  }

  log->info("Welcome to LabDB!");

  // Database directory:
  std::string dbdir = args["--dbdir"].asString();
  mkdir(dbdir.c_str(), 0755);  // ignore error if it exists
  
  // UUID:
  std::string theUUID = readOrMakeUUID(dbdir);
  log->info("My UUID is {}", theUUID);

  // Server config:
  ServerConfig config(theUUID);
  if (!config.parseURL(args["--bind"].asString())) {
    log->critical("Cannot parse URL given for --bind: {}",
                  args["--bind"].asString());
    _exit(2);
  }
  config.advAddr = args["--addr"] ? args["--addr"].asString() : "";
  config.pubAddr = args["--pub"] ? args["--pub"].asString() : config.advAddr;
  config.keyFile = args["--key"].asString();
  config.crtFile = args["--cert"].asString();

  // Peers:
  std::shared_ptr<Peers> peers 
      = std::make_shared<Peers>(theUUID,!args["--join"]);
  peers->update(std::make_shared<ServerConfig>(config));
  if (args["--join"]) {
    // Join the cluster here by sending a hello request
  }

  // Show command line arguments in log if verbose:
  if (args["--verbose"].asBool()) {
    log->info("Command line arguments:");
    for (auto const& p : args) {
      std::stringstream s;
      s << "Attribute: " << p.first << " Value: " << p.second;
      log->info(s.str());
    }
  }

  log->flush();

  // Start service, so far, this is crash only:
  runService(peers, args["--threads"].asLong(), log);

  return 0;
}
