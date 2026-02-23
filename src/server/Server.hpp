#ifndef SERVER_HPP
#define SERVER_HPP

#include "ConfigStructs.hpp"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

class Server {
public:
  Server(const std::vector<ServerConfig> &config);
  ~Server();

  void run();

private:
  const std::vector<ServerConfig> &_config;
};

#endif /* SERVER_HPP */
