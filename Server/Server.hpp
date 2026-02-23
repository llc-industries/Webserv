/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:26:19 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/21 01:12:08 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
// Nombre maximum d'événements traités par epoll_wait en une seule fois
#define MAX_EVENTS 64

// Structure qui lie un client (file descriptor) à sa requête HTTP.
struct ClientData {
  HttpRequest request;
};

class Server {
private:
  int _epoll_fd; // Le "cerveau" qui surveille tous les sockets
  std::map<int, ClientData>
      _clients; // Liste des clients connectés (FD -> Données)
  std::map<int, sockaddr_in> _servers; // Tableau rempli par epoll_wait
  struct epoll_event
      _events[MAX_EVENTS]; // Liste des sockets d'écoute (ports 8080, 9090...)
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
    SetupException(const std::string &msg) : _msg(msg) {}
    virtual ~SetupException() throw() {}
    virtual const char *what() const throw() { return _msg.c_str(); }
  };

  void setupServer(std::vector<int> ports);
  void run();
  void acceptConnection(int server_fd);
  void handleClient(int client_fd);
  void closeClient(int client_fd);
  void sendResponse(int client_fd);
  void parseRequestHeader(int client_fd);
  // std::string getContentType(const std::string& path);
};

#endif
