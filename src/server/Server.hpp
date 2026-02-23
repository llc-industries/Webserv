#ifndef SERVER_HPP
#define SERVER_HPP

#include "ConfigStructs.hpp"
#include <cstring>
#include <fcntl.h>
#include <map>
#include <netdb.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Server {
public:
  Server(const std::vector<ServerConfig> &config);
  ~Server();

  void run();
  void createSockets();

private:
  const std::vector<ServerConfig> &_config;

  typedef std::map<int, const ServerConfig *>::const_iterator it_sock;
  std::map<int, const ServerConfig *> _socketMap;

  void _socketFail(const std::string &funcName, struct addrinfo *res, int fd);
};

#endif /* SERVER_HPP */
