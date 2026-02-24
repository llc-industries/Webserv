#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "ConfigStructs.hpp"
#include "logs.hpp"

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <netdb.h>
#include <sstream>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 64

class Server {
public:
  Server(const std::vector<ServerConfig> &config);
  ~Server();

  void createSockets();
  void setupEpoll();
  void run();

  void acceptConnection(int fd, const ServerConfig *context);
  void closeClient(int fd);
  void handleClientRead(int fd);
  void handleClientWrite(int fd);

private:
  const std::vector<ServerConfig> &_config;

  std::map<int, const ServerConfig *> _socketMap;
  typedef std::map<int, const ServerConfig *>::iterator it_sock;

  int _epollFd;
  struct epoll_event _events[MAX_EVENTS];

  std::map<int, Client> _clientMap;

  void _socketFail(const std::string &funcName, struct addrinfo *res, int fd);
};

#endif /* SERVER_HPP */
