// peers.h - simple peer to peer network setup

#ifndef LABDB_PEERS_H
#define LABDB_PEERS_H 1

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "serverconfig.h"

class Peers {
  std::mutex _mutex;
  std::string _myUUID;
  std::map<std::string, std::shared_ptr<ServerConfig>> _peers; 
      // map from UUID to the config
  bool _iAmLeader;

 public:
  Peers(std::string const& myUUID, bool iAmLeader)
    : _myUUID(myUUID), _iAmLeader(iAmLeader) {
  }

  std::string const& myUUID() {
    return _myUUID;
  }

  void update(std::shared_ptr<ServerConfig> conf) {
    std::unique_lock<std::mutex> guard(_mutex);
    auto i = _peers.find(conf->myUUID);
    if (i != _peers.end()) {
      i->second = conf;
    } else {
      _peers.emplace(conf->myUUID, conf);
    }
  }

  std::shared_ptr<ServerConfig> find(std::string const& UUID) {
    std::unique_lock<std::mutex> guard(_mutex);
    auto i = _peers.find(UUID);
    if (i != _peers.end()) {
      return i->second;
    } else {
      return nullptr;
    }
  }
};

#endif
