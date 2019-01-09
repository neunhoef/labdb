#include "uuid.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


std::string readOrMakeUUID(std::string const& dbdir) {
  std::string path = dbdir + "/UUID";
  std::string theUUID;
  // Check on disk:
  std::fstream f(path.c_str(), std::ios::in);
  if (!f.fail() && !f.eof()) {
    getline(f, theUUID);
    if (!theUUID.empty()) {
      return theUUID;
    }
  }
  // Obviously, file does not exist or cannot be read or is empty, so
  // create a UUID and persist:
  std::stringstream ss;
  ss << boost::uuids::random_generator()();
  theUUID = ss.str();

  f.open(path.c_str(), std::ios::out);
  if (f.fail()) {
    std::cerr << "Cannot persist UUID, giving up!" << std::endl;
    _exit(1);
  }
  f << theUUID << std::endl;
  f.close();
  if (f.fail()) {
    std::cerr << "Cannot close UUID file, giving up!" << std::endl;
    _exit(2);
  }
  return theUUID;
}

