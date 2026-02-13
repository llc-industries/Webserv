#include "ConfigPrint.hpp"

typedef std::vector<int>::const_iterator it_int;
typedef std::vector<std::string>::const_iterator it_str;
typedef std::map<int, std::string>::const_iterator it_map;

ConfigPrint::ConfigPrint(const std::vector<ServerConfig> &config)
    : _config(config) {

  for (size_t i = 0; i < config.size(); i++) {
    LOG_CONFIG(GREEN " ======== [ Server block " << (i + 1)
                                                 << " ] ========" RESET);
    _printServerBlock(config[i]);
    for (size_t j = 0; j < config[i].locations.size(); j++) {
      LOG_CONFIG(YELLOW " ==== [ Location " << config[i].locations[j].path
                                            << " ] ====" RESET);
      _printLocationBlock(config[i].locations[j]);
    }
  }
}

void ConfigPrint::_printServerBlock(const ServerConfig &sc) const {
  std::stringstream ss;

  LOG_CONFIG("Host ----------> " << sc.host);

  for (it_int it = sc.ports.begin(); it != sc.ports.end(); ++it)
    ss << *it << ' ';
  LOG_CONFIG("Ports ---------> " << ss.str());
  ss.str(std::string(""));

  if (sc.serverName.empty() == false)
    LOG_CONFIG("Server name ---> " << sc.serverName);

  LOG_CONFIG("Root ----------> " << sc.root);

  if (sc.index.empty() == false) {
    for (it_str it = sc.index.begin(); it != sc.index.end(); ++it)
      ss << *it << ' ';
    LOG_CONFIG("Index ---------> " << ss.str());
    ss.str(std::string(""));
  }

  LOG_CONFIG("Max body size -> " << sc.maxBodySize);

  if (sc.errPages.empty() == false) {
    for (it_map it = sc.errPages.begin(); it != sc.errPages.end(); ++it)
      ss << it->first << " " << it->second << " ## ";
    LOG_CONFIG("Error pages ---> " << ss.str());
  }
}

void ConfigPrint::_printLocationBlock(const Location &loc) const {
  std::stringstream ss;

  if (loc.root.empty() == false)
    LOG_CONFIG("Root --------> " << loc.root);

  if (loc.index.empty() == false) {
    for (it_str it = loc.index.begin(); it != loc.index.end(); ++it)
      ss << *it << ' ';
    LOG_CONFIG("Index -------> " << ss.str());
    ss.str(std::string(""));
  }

  if (loc.methods.empty() == false) {

    for (it_str it = loc.methods.begin(); it != loc.methods.end(); ++it)
      ss << *it << ' ';
    LOG_CONFIG("Methods -----> " << ss.str());
    ss.str(std::string(""));
  }

  if (loc.ret.first != -1)
    LOG_CONFIG("Return ------> " << loc.ret.first << " " << loc.ret.second);
  if (loc.cgiPath.empty() == false)
    LOG_CONFIG("CGI Path ----> " << loc.cgiPath);
  if (loc.uploadPath.empty() == false)
    LOG_CONFIG("Upload Path -> " << loc.uploadPath);
  LOG_CONFIG("Autoindex ---> " << (loc.autoindex == 0 ? "false" : "true"));
}
