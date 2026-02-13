#include "ConfigPrint.hpp"

ConfigPrint::ConfigPrint(const std::vector<ServerConfig> &config)
    : _config(config) {

  for (size_t i = 0; i < config.size(); i++) {
    LOG_CONFIG(" ======== [ Server block " << i << " ] ========");
    _printServerBlock(config[i]);
    for (size_t j = 0; j < config[i].locations.size(); j++) {
      LOG_CONFIG(" ==== [ Location block " << j << " ] ====");
      _printLocationBlock(config[i].locations[j]);
    }
  }
}

void ConfigPrint::_printServerBlock(const ServerConfig &sc) const {
  LOG_CONFIG("Ports");
  LOG_CONFIG("fwe");
}

void ConfigPrint::_printLocationBlock(const Location &loc) const {
  LOG_CONFIG("Ports");
  LOG_CONFIG("fwe");
}
