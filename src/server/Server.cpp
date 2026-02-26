#include "Server.hpp"

Server::Server(const std::vector<ServerConfig> &config)
    : _config(config), _epollFd(-1) {}

Server::~Server() {
  for (it_sock it = _socketMap.begin(); it != _socketMap.end(); ++it)
    close(it->first);
  if (_epollFd != -1)
    close(_epollFd);
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
  LOG_INFO(_socketMap.size() << " Sockets created");
}
// clang-format on

void Server::setupEpoll() {
  _epollFd = epoll_create(_socketMap.size());
  if (_epollFd == -1)
    throw std::runtime_error("epoll_create(): " + std::string(strerror(errno)));

  for (it_sock it = _socketMap.begin(); it != _socketMap.end(); ++it) {
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));

    ev.events = EPOLLIN;
    ev.data.fd = it->first;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, it->first, &ev) != 0)
      throw std::runtime_error("epoll_ctl(): " + std::string(strerror(errno)));
  }
  LOG_INFO("epoll instance is ready");
}

void Server::run() {
  LOG_INFO("---------- Server is running ! ----------");
  while (42) {
    int nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
    if (nfds == -1)
      throw std::runtime_error("epoll_wait(): " + std::string(strerror(errno)));

    for (int i = 0; i < nfds; i++) {
      int currentFd = _events[i].data.fd;
      uint32_t eventMask = _events[i].events;
      it_sock it = _socketMap.find(currentFd);

      if (it != _socketMap.end()) { // New client
        acceptConnection(currentFd, it->second);
      } else { // Existing client
        if (eventMask & (EPOLLERR | EPOLLHUP))
          closeClient(currentFd);
        else if (eventMask & EPOLLIN) // recv()
          handleClientRead(currentFd);
        else if (eventMask & EPOLLOUT) // send()
          handleClientWrite(currentFd);
      }
    }
  }
}

void Server::acceptConnection(int fd, const ServerConfig *config) {
  sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  int client_fd = accept(fd, (sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1) {
    LOG_ERR("accept(): " + std::string(strerror(errno)));
    return;
  }
  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
    LOG_ERR("fcntl(): " + std::string(strerror(errno)));
    close(client_fd);
    return;
  }

  epoll_event ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = client_fd;

  if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
    LOG_ERR("epoll_ctl(): " + std::string(strerror(errno)));
    close(client_fd);
    return;
  }

  _clientMap.insert(std::make_pair(client_fd, Client(config)));

  // TODO: Format IP address clean -> byte.byte.byte.byte:port
  LOG_ACCEPT("New client. FD = " << client_fd << " IP/Port = "
                                 << ntohl(client_addr.sin_addr.s_addr) << ":"
                                 << ntohs(client_addr.sin_port));
}

void Server::closeClient(int client_fd) {
  epoll_ctl(_epollFd, EPOLL_CTL_DEL, client_fd, NULL);
  close(client_fd);
  _clientMap.erase(client_fd);
  LOG_CLOSE("Closed client connection. FD = " << client_fd);
}

void Server::handleClientRead(int fd) {
  char buf[4096];
  ssize_t retval = recv(fd, buf, sizeof(buf), 0); // J'ai remove le -1 du sizeof
  if (retval == -1) {
    LOG_ERR("recv(): " + std::string(strerror(errno)));
    closeClient(fd);
    return;
  } else if (retval == 0) {
    closeClient(fd);
    return;
  }
  _clientMap[fd].swallow(buf, retval);
  if (_clientMap[fd].isRequestComplete() == true) {
    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
  }
}

// TODO: ne pas envoyer tout d'un coup en mode bourrin
void Server::handleClientWrite(int fd) {
  Client &client = _clientMap[fd];

  client.buildResponse();
  if (client.isResponseReady() == true) {
    if (send(fd, client.getResponse(), client.getResponseLength(), 0) == -1)
      LOG_ERR("send(): " + std::string(strerror(errno)));
    closeClient(fd);
  }
}

void Server::_socketFail(const std::string &funcName, struct addrinfo *res,
                         int fd) {
  if (fd != -1)
    close(fd);
  if (res != NULL)
    freeaddrinfo(res);
  throw std::runtime_error(funcName + "(): " + strerror(errno));
}
