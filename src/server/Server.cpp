#include "Server.hpp"

Server::Server(const std::vector<ServerConfig> &config) : _config(config) {}

Server::~Server() {
  for (it_sock it = _socketMap.begin(); it != _socketMap.end(); ++it)
    close(it->first);
}

// clang-format off
// Create sockets and associate them with config context
void Server::createSockets() {
  // Init hints for getaddrinfo()
  struct addrinfo hints;
  std::memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Loop over server blocks
  for (size_t i = 0; i < _config.size(); i++) {
    const ServerConfig &curServ = _config[i];
    const std::vector<int> &curPortVec = curServ.ports;

    // Nested loop over server ports
    for (size_t j = 0; j < curPortVec.size(); j++) {
      struct addrinfo *res = NULL;
      std::stringstream portStr;
      portStr << curPortVec[j];

      int retval = getaddrinfo(curServ.host.c_str(), portStr.str().c_str(), &hints, &res);
      if (retval != 0 || res == NULL)
        throw std::runtime_error("getaddrinfo(" + curServ.host + ", " + portStr.str() + ") -> " + gai_strerror(retval));

      int socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (socketfd == -1)
        _socketFail("socket", res, socketfd);

      int opt = 1;
      if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        _socketFail("setsockopt", res, socketfd);

      if (fcntl(socketfd, F_SETFL, O_NONBLOCK) == -1 )
        _socketFail("fcntl", res, socketfd);

      if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1)
        _socketFail("bind", res, socketfd);

      if (listen(socketfd, SOMAXCONN) == -1)
        _socketFail("listen", res, socketfd);

      _socketMap[socketfd] = &curServ;
      freeaddrinfo(res);
    }
  }
}
// clang-format on

void Server::_socketFail(const std::string &funcName, struct addrinfo *res,
                         int fd) {
  if (fd != -1)
    close(fd);
  freeaddrinfo(res);
  throw std::runtime_error(funcName + "(): " + strerror(errno));
}

void Server::run() {}
