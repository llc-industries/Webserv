@pragma once

#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 64

    class Server {
private:
  int _epoll_fd;
  std::map <int, ClientData> _clients; 
  std::map<int, sockaddr_in>
      _servers;
  struct epoll_event _events[MAX_EVENTS];
  std::vector<int> _server_fds;
  int _port;

public:
    Server(int port = 8080);
    ~Server();

  void setupServer(std::vector<int> ports);
  void run();
  void acceptConnection(int server_fd);
  void handleClient(int client_fd);
  void closeClient(int client_fd);
  void sendResponse(int client_fd);
};