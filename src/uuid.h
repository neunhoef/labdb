// Create and persist a UUID to identify the instance

#ifndef LABDB_UUID_H
#define LABDB_UUID_H 1

#include <string>

// The following will read the UUID from the file ./UUID if it exists.
// If the file does not exist in the database directory, it will create a
// UUID, persist it and cache it in memory. The final value is returned.

std::string readOrMakeUUID(std::string const& dbdir);

#endif
