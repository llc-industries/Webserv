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

  static bool stopSignal;
  static void sigintHandler(int _unused);

  void createSockets();
  void setupEpoll();
  void run();

private:
  Server(const Server &other);
  Server &operator=(const Server &other);

  typedef std::map<int, const ServerConfig *>::iterator it_sock;

  void _acceptConnection(int fd, const ServerConfig *context);
  void _closeClient(int client_fd);
  void _handleClientRead(int fd);
  void _handleClientWrite(int fd);

  void _socketFail(const std::string &funcName, struct addrinfo *res, int fd);

  // Config
  const std::vector<ServerConfig> &_config;

  // Epoll instance
  std::map<int, const ServerConfig *> _socketMap;
  int _epollFd;
  struct epoll_event _events[MAX_EVENTS];

  // Current clients
  std::map<int, Client> _clientMap;
};

#endif /* SERVER_HPP */
