#include "logs.hpp"
#include "parser/ConfigParser.hpp"
#include "parser/ConfigPrint.hpp"
#include "server/Server.hpp"

#include <iostream>
#include <stdlib.h>

#define DEFAULT_CONF "conf/test.conf"

std::string findConfigFile(int argc, char **argv) {
  std::string configPath;

  if (argc < 2) {
    LOG_INFO("Using " DEFAULT_CONF " as config file");
    configPath = DEFAULT_CONF;
  } else if (argc > 2) {
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
    ConfigPrint print(config);
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
    return EXIT_FAILURE;
  }

  try {
    Server server(config);
    signal(SIGPIPE, SIG_IGN);
    server.createSockets();
    server.setupEpoll();
    server.run();
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
    return EXIT_FAILURE;
  }

  std::cout << "\nBye" << std::endl;
  return EXIT_SUCCESS;
}
