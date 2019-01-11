// config.cpp - configuration of a single server

#include "serverconfig.h"

#include <http_parser.h>

bool ServerConfig::parseURL(std::string const& url) {
  struct http_parser_url result;
  http_parser_url_init(&result);
  int r = http_parser_parse_url(url.c_str(), url.size(), false, &result);
  if (r != 0) {
    encryption = false;
    bindAddr = "";
    bindPort = "";
    return false;
  }
  encryption = url.substr(result.field_data[UF_SCHEMA].off,
                          result.field_data[UF_SCHEMA].len) == "https";
  bindAddr = url.substr(result.field_data[UF_HOST].off,
                        result.field_data[UF_HOST].len);
  bindPort = url.substr(result.field_data[UF_PORT].off,
                        result.field_data[UF_PORT].len);
  return true;
}

void ServerConfig::toBuilder(VPackBuilder& b) {
  { VPackObjectBuilder guard(&b);
    b.add("myUUID",     VPackValue(myUUID));
    b.add("encryption", VPackValue(encryption));
    b.add("bindAddr",   VPackValue(bindAddr));
    b.add("bindPort",   VPackValue(bindPort));
    b.add("advAddr",    VPackValue(advAddr));
    b.add("pubAddr",    VPackValue(pubAddr));
    b.add("keyFile",    VPackValue(keyFile));
    b.add("crtFile",    VPackValue(crtFile));
  }
}

bool ServerConfig::fromSlice(VPackSlice s) {
  if (!s.isObject()) {
    return false;
  }

  VPackSlice ss = s.get("myUUID");
  if (!ss.isString()) {
    return false;
  }
  myUUID = ss.copyString();

  ss = s.get("encryption");
  if (!ss.isBool()) {
    return false;
  }
  encryption = ss.isTrue();

  ss = s.get("bindAddr");
  if (!ss.isString()) {
    return false;
  }
  bindAddr = ss.copyString();

  ss = s.get("bindPort");
  if (!ss.isString()) {
    return false;
  }
  bindPort = ss.copyString();

  ss = s.get("advAddr");
  if (!ss.isString()) {
    return false;
  }
  advAddr = ss.copyString();

  ss = s.get("pubAddr");
  if (!ss.isString()) {
    return false;
  }
  pubAddr = ss.copyString();

  ss = s.get("keyFile");
  if (!ss.isString()) {
    return false;
  }
  keyFile = ss.copyString();

  ss = s.get("crtFile");
  if (!ss.isString()) {
    return false;
  }
  crtFile = ss.copyString();

  return true;
}
