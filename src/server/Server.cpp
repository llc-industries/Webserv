#include "Server.hpp"

bool Server::stopSignal = false;

Server::Server(const std::vector<ServerConfig> &config)
    : _config(config), _epollFd(-1) {}

Server::~Server() {
  for (it_sock it = _socketMap.begin(); it != _socketMap.end(); ++it)
    close(it->first);
  if (_epollFd != -1)
    close(_epollFd);
}

void Server::sigintHandler(int unused) {
  (void)unused;
  Server::stopSignal = true;
}

/* -------- PUBLIC METHODS -------- */

// clang-format off
// Create sockets and associate them with config context
void Server::createSockets() {
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
}

void Server::run() {
  if (signal(SIGINT, sigintHandler) == SIG_ERR)
    throw std::runtime_error("Couldn't setup SIGINT (CTRL+C)");
  LOG_INFO("---------- Server is running (" << _socketMap.size()
                                            << " socket) ----------");

  while (Server::stopSignal == false) {
    int nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, EPOLL_TIMEOUT);
    if (nfds == -1 && Server::stopSignal == false)
      throw std::runtime_error("epoll_wait(): " + std::string(strerror(errno)));

    for (int i = 0; i < nfds; i++) {
      int currentFd = _events[i].data.fd;
      uint32_t eventMask = _events[i].events;
      it_sock it = _socketMap.find(currentFd);
      std::map<int, int>::iterator it_cgi = _cgiMap.find(currentFd);

      if (it != _socketMap.end()) { // New client
        _acceptConnection(currentFd, it->second);
      } else if (it_cgi != _cgiMap.end()) { // Handle cgi pipes
        int client_fd = it_cgi->second;
        Client &client = _clientMap.find(client_fd)->second;

        if (currentFd == client.getCgiFdOut()) {
          if (eventMask & (EPOLLIN | EPOLLHUP | EPOLLERR))
            _handleCgiRead(currentFd);
        }
        if (currentFd == client.getCgiFdIn()) {
          if (eventMask & (EPOLLOUT | EPOLLHUP | EPOLLERR))
            _handleCgiWrite(currentFd);
        }
      } else { // Existing client
        if (eventMask & (EPOLLERR | EPOLLHUP))
          _closeClient(currentFd);
        else if (eventMask & EPOLLIN) // recv()
          _handleClientRead(currentFd);
        else if (eventMask & EPOLLOUT) // send()
          _handleClientWrite(currentFd);
      }
    }
    _handleTimeouts();
  }
}

/* -------------- PRIVATE METHODS -------------- */

void Server::_acceptConnection(int fd, const ServerConfig *config) {
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
  LOG_ACCEPT(client_fd, "New connection");
}

void Server::_closeClient(int client_fd) {
  Client &client = _clientMap.find(client_fd)->second;

  if (client.getCgiPid() != -1) { // Clean CGI
    kill(client.getCgiPid(), SIGKILL);
    waitpid(client.getCgiPid(), NULL, WNOHANG);
  }

  int cgiFdOut = client.getCgiFdOut();
  if (cgiFdOut != -1) {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgiFdOut, NULL);
    client.closeCgiFdOut();
    _cgiMap.erase(cgiFdOut);
  }

  int cgiFdIn = client.getCgiFdIn();
  if (cgiFdIn != -1) {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgiFdIn, NULL);
    client.closeCgiFdIn();
    _cgiMap.erase(cgiFdIn);
  }

  epoll_ctl(_epollFd, EPOLL_CTL_DEL, client_fd, NULL); // Clean _clientMap
  close(client_fd);
  _clientMap.erase(client_fd);
  LOG_CLOSE(client_fd, "Closed client connection");
}

void Server::_handleClientRead(int fd) {
  char buf[4096];
  ssize_t retval = recv(fd, buf, sizeof(buf), 0);
  if (retval == -1) {
    LOG_ERR("recv(): " + std::string(strerror(errno)));
    _closeClient(fd);
    return;
  } else if (retval == 0) {
    _closeClient(fd);
    return;
  }
  Client &client = _clientMap.find(fd)->second;
  client.swallow(buf, retval);
  if (client.isRequestComplete() == true) {
    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
  }
}

void Server::_handleClientWrite(int fd) {
  Client &client = _clientMap.find(fd)->second;
  if (client.isResponseReady() == false) {
    client.buildResponse();

    if (client.getCgiPid() != -1) {
      _registerCgi(fd);
      return;
    }
  }
  if (client.isResponseReady() == true) {
    ssize_t retval = 0;
    size_t rest = client.getResponseStrLength() - client.getBytesSent();

    retval = send(fd, client.getResponseStr() + client.getBytesSent(), rest, 0);
    if (retval <= 0) {
      LOG_ERR("send(): " + std::string(strerror(errno)));
      _closeClient(fd);
      return;
    } else {
      client.addBytesSent(retval);
    }

    if (client.getBytesSent() >= client.getResponseStrLength()) {

      std::string agent = client.getRequest().getHeader("User-Agent");
      if (agent.empty() == true)
        agent = "Unknown agent";

      LOG_HTTP(
          fd, client.getRequest().getMethod(), client.getRequest().getPath(),
          client.getResponse().getStatusCode(), client.getBytesSent(), agent);
      _closeClient(fd);
    }
  }
}

void Server::_handleTimeouts() {
  std::time_t now = std::time(NULL);
  it_client it = _clientMap.begin();

  while (it != _clientMap.end()) {
    Client &client = it->second;
    pid_t pid = client.getCgiPid();
    std::time_t lastAct = client.getLastActivity();

    if (now - lastAct >= CLIENT_TIMEOUT && pid == -1) { // Timeout classique
      int fdSave = it->first;
      ++it;
      LOG_CLOSE(fdSave, "Client has timed out");
      _closeClient(fdSave);
    } else if (now - lastAct >= CLIENT_TIMEOUT && pid != -1) { // Timeout de cgi
      // Kill le CGI et prépare l'envoi d'un 502 du client
      kill(pid, SIGKILL);
      waitpid(pid, NULL, WNOHANG);

      int cgiFdOut = client.getCgiFdOut();
      if (cgiFdOut != -1) {
        epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgiFdOut, NULL);
        close(cgiFdOut);
        _cgiMap.erase(cgiFdOut);
      }

      client.cgiTimeoutClean();

      epoll_event ev;
      std::memset(&ev, 0, sizeof(ev));
      ev.events = EPOLLOUT;
      ev.data.fd = it->first;
      epoll_ctl(_epollFd, EPOLL_CTL_MOD, it->first, &ev);

      ++it;
    } else
      ++it;
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

void Server::_registerCgi(int client_fd) {
  Client &client = _clientMap.find(client_fd)->second;
  int cgiFdOut = client.getCgiFdOut();
  int cgiFdIn = client.getCgiFdIn();

  if (cgiFdOut != -1) {
    _cgiMap[cgiFdOut] = client_fd;

    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = cgiFdOut;
    epoll_ctl(_epollFd, EPOLL_CTL_ADD, cgiFdOut, &ev);
  }

  if (cgiFdIn != -1) {
    _cgiMap[cgiFdIn] = client_fd;

    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = cgiFdIn;
    epoll_ctl(_epollFd, EPOLL_CTL_ADD, cgiFdIn, &ev);
  }

  epoll_event ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.events = 0;
  ev.data.fd = client_fd;
  epoll_ctl(_epollFd, EPOLL_CTL_MOD, client_fd, &ev);
}

void Server::_handleCgiRead(int cgi_fd) {
  int client_fd = _cgiMap[cgi_fd];
  Client &client = _clientMap.find(client_fd)->second;
  client.updateActivity();

  char buf[4096];
  ssize_t bytes = read(cgi_fd, buf, sizeof(buf));

  if (bytes > 0) {
    client.appendCgiOutput(buf, bytes);
  } else if (bytes == 0) {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgi_fd, NULL);
    client.closeCgiFdOut();
    _cgiMap.erase(cgi_fd);

    int status = 0;

    waitpid(client.getCgiPid(), &status, WNOHANG);
    client.resetCgi();

    if (WIFSIGNALED(status) ||
        (WIFEXITED(status) && WEXITSTATUS(status) != 0)) {
      LOG_ERR("CGI Process return value " << WEXITSTATUS(status));
      client.cgiCrash();
    } else
      client.parseCgiResponse();

    epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = client_fd;
    epoll_ctl(_epollFd, EPOLL_CTL_MOD, client_fd, &ev);
  } else {
    LOG_ERR("CGI read error");
    _closeClient(client_fd);
  }
}

void Server::_handleCgiWrite(int cgi_fd) {
  int client_fd = _cgiMap[cgi_fd];
  Client &client = _clientMap.find(client_fd)->second;
  client.updateActivity();
  const std::string &body = client.getRequestBody();

  size_t rest = body.length() - client.getCgiBytesWritten();
  if (rest > 0) {
    ssize_t retval =
        write(cgi_fd, body.c_str() + client.getCgiBytesWritten(), rest);
    if (retval > 0) {
      client.addCgiBytesWritten(retval);
      rest -= retval;
    } else if (retval == -1) {
      LOG_ERR("Cgi write error");
      _closeClient(client_fd);
      return;
    }
  }

  if (rest == 0) {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgi_fd, NULL);
    _cgiMap.erase(cgi_fd);
    client.closeCgiFdIn();
  }
}
