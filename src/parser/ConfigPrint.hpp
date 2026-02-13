#ifndef CONFIG_PRINT_H
#define CONFIG_PRINT_H

#include "ConfigStructs.hpp"
#include "logs.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"

class ConfigPrint {
public:
  ConfigPrint(const std::vector<ServerConfig> &config);
  ~ConfigPrint() {}

private:
  const std::vector<ServerConfig> &_config;

  void _printServerBlock(const ServerConfig &sc) const;
  void _printLocationBlock(const Location &loc) const;
};

#endif /* CONFIG_PRINT_H */
