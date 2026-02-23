#include "logs.hpp"
#include "parser/ConfigParser.hpp"
#include "parser/ConfigPrint.hpp"
#include "server/Server.hpp"

#include <iostream>
#include <stdlib.h>

std::string findConfigFile(int argc, char **argv) {
  std::string configPath;

  if (argc < 2) {
    LOG_WARN("No arguments provided, using default configuration file");
    LOG_INFO("Using conf/testserver.conf as config file");
    configPath = "conf/testserver.conf";
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

  std::vector<ServerConfig> config;
  const std::string configPath = findConfigFile(argc, argv);

  try {
    ConfigParser parser(configPath);
    config = parser.getConfig();
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
    return EXIT_FAILURE;
  }

  ConfigPrint Print(config);

  try {
    Server server(config);
    server.run();
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
