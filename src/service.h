// service.h - run the HTTP/2 service

#ifndef LABDB_SERVICE_H
#define LABDB_SERVICE_H 1

#include <string>
#include <spdlog/spdlog.h>

#include "peers.h"

void runService(std::shared_ptr<Peers> peers,
                std::size_t num_threads,
                std::shared_ptr<spdlog::logger> log);

#endif
