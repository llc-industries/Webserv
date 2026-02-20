/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:26:19 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/20 23:48:18 by atazzit          ###   ########.fr       */
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
#include <cstdio>
#include <exception>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#define MAX_EVENTS 64

struct ClientData {
  HttpRequest request;
};

class Server {
private:
  int _epoll_fd;
  std::map<int, ClientData> _clients;
  std::map<int, sockaddr_in> _servers;
  struct epoll_event _events[MAX_EVENTS];
  std::vector<int> _server_fds;
  int _port;
  
  void handleGet(int client_fd);
  void handlePost(int client_fd);
  void handleDelete(int client_fd);

public:
  Server(int port = 8080);
  ~Server();
  class SetupException : public std::exception {
  private:
      std::string _msg;
  public:
      SetupException(const std::string& msg) : _msg(msg) {}
      virtual ~SetupException() throw() {}
      virtual const char* what() const throw() { return _msg.c_str(); }
  };
  
  void setupServer(std::vector<int> ports);
  void run();
  void acceptConnection(int server_fd);
  void handleClient(int client_fd);
  void closeClient(int client_fd);
  void sendResponse(int client_fd);
  void parseRequestHeader(int client_fd);
  std::string getContentType(const std::string& path);
};

#endif