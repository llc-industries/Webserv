/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:26:19 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/15 16:48:31 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <fstream>
#include <sstream>
#include <arpa/inet.h> 
#include <string>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>

#define MAX_EVENTS 64

struct ClientData
{
  std::string response;
  std::string request;

  std::string method;
  std::string path;
  std::string host;
  
  bool header_parsed;

  int content_length;
  
  ClientData() : header_parsed(false), content_length(0) {}
};


class Server {
private:
  int _epoll_fd;
  std::map<int, ClientData> _clients;
  std::map<int, sockaddr_in> _servers;
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
  void parseRequestHeader(int client_fd);
};

#endif