#include "uuid.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


static std::string theUUID;

std::string getUUID() {
  if (!theUUID.empty()) {
    return theUUID;
  }
  // Check on disk:
  std::fstream f("./UUID", std::ios::in);
  if (!f.fail() && !f.eof()) {
    getline(f, theUUID);
    if (!theUUID.empty()) {
      return theUUID;
    }
  }
  // Obviously, file does not exist or cannot be read, so create a UUID
  // and persist:
  std::stringstream ss;
  ss << boost::uuids::random_generator()();
  theUUID = ss.str();

  f.open("./UUID", std::ios::out);
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
