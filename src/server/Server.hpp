#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "ConfigStructs.hpp"
#include "logs.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_EVENTS 64

class Server {
public:
  Server(const std::vector<ServerConfig> &config);
  ~Server();

  void createSockets();
  void setupEpoll();
  void run();

  void acceptConnection(int fd, const ServerConfig *context);
  void closeClient(int client_fd);
  void handleClientRead(int fd);
  void handleClientWrite(int fd);

  static void sigintHandler(int _unused);
  static bool stopSignal;

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
