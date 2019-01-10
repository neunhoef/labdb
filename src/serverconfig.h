// config.h - configuration of a single server

#ifndef LABDB_SERVERCONFIG_H
#define LABDB_SERVERCONFIG_H 1

#include <string>

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

struct ServerConfig {
  bool        encryption; // true for TLS
  std::string bindAddr;   // bind address
  std::string bindPort;   // bind port
  std::string advAddr;    // advertised address internally as URL
  std::string pubAddr;    // advertised address for extern access as URL
  std::string keyFile;    // name for TLS key file
  std::string crtFile;    // name for TLS crt file

  ServerConfig() : encryption(true) {}

  bool parseURL(std::string const& url);
  // Parses an URL for encryption, bindAddr and bindPort, ignores others.
  // Returns true for good and false for error.
  
  void toBuilder(VPackBuilder& b);
  // Dumps to velocypack, writes exactly one object

  bool fromSlice(VPackSlice s);
  // Restores from velocypack, returns true if OK and false otherwise
};

#endif
