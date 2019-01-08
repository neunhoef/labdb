// Create and persist a UUID to identify the instance

#include <string>

// The following will read the UUID from the file ./UUID if it exists,
// and cache it in memory. If the file does not exist in the current
// directory, it will create a UUID, persist it and cache it in memory.
// The final value is returned.

std::string getUUID();
