#include "ConfigParser.hpp"
#include "logs.hpp"

#include <iostream>
#include <stdlib.h>

std::string findConfigFile(int argc, char **argv) {
  std::string configPath;

  if (argc < 2) {
    LOG_WARN("No arguments provided, using default configuration file");
    LOG_INFO("Using conf/webserv.conf as config file");
    configPath = "conf/webserv.conf";
  } else if (argc > 2) {
    LOG_WARN("Too many arguments provided");
    LOG_INFO("Using \"" << argv[1] << "\" as config file");
    configPath = argv[1];
  } else {
    LOG_INFO("Using \"" << argv[1] << "\" as config file");
    configPath = argv[1];
  }

  return configPath;
}

int main(int argc, char **argv) {
  PRINT_HEADER;
  const std::string configPath = findConfigFile(argc, argv);

  try {
    ConfigParser Config(configPath);
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
  }

  return EXIT_SUCCESS;
}
