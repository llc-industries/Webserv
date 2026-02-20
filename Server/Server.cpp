/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 15:34:07 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/21 00:40:50 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : _epoll_fd(-1), _port(port) {
  (void)_port;
}

Server::~Server() {
  for (size_t i = 0; i < _server_fds.size(); i++)
    close(_server_fds[i]);
  if (_epoll_fd != -1)
    close(_epoll_fd);
}

void Server::setupServer(std::vector<int> ports) {
  _epoll_fd = epoll_create1(0);
  if (_epoll_fd == -1) {
    throw SetupException("epoll_create1 failed");
  }
  for (size_t i = 0; i < ports.size(); i++){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      throw SetupException("Socket creation failed");
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
      throw SetupException("socket option failed (re-use address)");
    }
      
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(ports[i]);
    address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0) {
      close(server_fd);
      throw SetupException("bind faailed");
    }
    if (listen(server_fd, SOMAXCONN) < 0) {
      close(server_fd);
      throw SetupException("listen failed");
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
      close(server_fd);
      throw SetupException("epoll control failed");
    }
    _server_fds.push_back(server_fd);
  }
}

void Server::run() {
    std::cout << "[SERVER] Starting server..." << std::endl;

    while (true) {
        int nfds = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
        
        if (nfds == -1) {
            std::cerr << "[ERROR] epoll_wait failed" << std::endl;
            break;
        }
        for (int i = 0; i < nfds; i++) {
            int fd = _events[i].data.fd;
            bool is_server = false;//si c'est un serveur (socket d'écoute)
            for (size_t j = 0; j < _server_fds.size(); j++) {
                if (_server_fds[j] == fd) {
                    is_server = true;
                    break;
                }
            }
            if (is_server) {
                acceptConnection(fd);
            } else if (_events[i].events & EPOLLIN) {
                handleClient(fd);
            } else if (_events[i].events & EPOLLOUT) {
                sendResponse(fd);
            }
        }
    }
}

void Server::acceptConnection(int server_fd) {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_len);
    
    if (client_fd == -1) {
        std::cerr << "[ERROR] accept failed" << std::endl;
        return;
    }
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
   _clients[client_fd] = ClientData();

    std::cout << "[CONNECTION] Client connected: " << inet_ntoa(client_addr.sin_addr) 
              << ":" << ntohs(client_addr.sin_port) << std::endl;
}

void Server::handleClient(int client_fd)
{
  char buffer[4096];
  ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes <= 0)
  {
    if (bytes < 0) std::cerr << "[ERROR] recv failed \n";
    closeClient(client_fd);
    return;
  }
    _clients[client_fd].request.swallow(buffer, bytes);
     
      if (_clients[client_fd].request.isComplete()){
        epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD ,client_fd, &ev);
      } 
}

void Server::closeClient(int client_fd)
{
  epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
  close(client_fd);
  _clients.erase(client_fd);
  std::cout << "[INFO] Connection closed for fd:" << client_fd << "\n";
}

void Server::sendResponse(int client_fd) {
  ClientData &client = _clients[client_fd];
  std::string method = client.request.getMethod();

  if (method == "GET") {
      handleGet(client_fd);
  } else if (method == "POST") {
      handlePost(client_fd);
  } else if (method == "DELETE") {
      handleDelete(client_fd);
  } else {
      // Si la méthode n'est pas reconnue
      HttpResponse res;
      res.setStatusCode(405);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
      std::string full_res = res.toString();
      send(client_fd, full_res.c_str(), full_res.size(), 0);
      closeClient(client_fd);
  }
}

void Server::handleGet(int client_fd) {
  ClientData &client = _clients[client_fd];
  std::string root = "./www";
  std::string path = client.request.getPath();
  std::string full_path = root + path;

  if (path == "/")
    full_path = root + "/index.html";
  
  std::ifstream file(full_path.c_str(), std::ios::binary);
  HttpResponse res;
  
  if (!file.is_open()) {
    res.setStatusCode(404);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
  } else {
    std::ostringstream file_content;
    file_content << file.rdbuf();
  
    res.setStatusCode(200);
    res.autoDetectContentType(full_path);
    res.setBody(file_content.str());
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}

void Server::handlePost(int client_fd) {
  ClientData &client = _clients[client_fd];
 std::string body_content = client.request.getBody();
  
  std::string save_path = "./www/uploaded_file.txt";
  std::ofstream outfile(save_path.c_str(), std::ios::binary);
  HttpResponse res;
  
  if (outfile.is_open()) {
    outfile.write(body_content.c_str(), body_content.size());
    outfile.close();
    
    res.setStatusCode(201);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>Upload reussi ! Fichier sauvegarde.</h1></body></html>");
  } else {
    res.setStatusCode(500);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}


void Server::handleDelete(int client_fd) {
ClientData &client = _clients[client_fd];
  std::string root = "./www";
  std::string full_path = root + client.request.getPath();
  HttpResponse res;

  if (std::remove(full_path.c_str()) == 0) {
      res.setStatusCode(200);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>200 OK : Fichier supprime avec succes !</h1></body></html>");
  } else {
      res.setStatusCode(404);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>404 Not Found (ou droits insuffisants)</h1></body></html>");
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}

// std::string Server::getContentType(const std::string& path) {
//   size_t dotPos = path.find_last_of(".");
//   if (dotPos == std::string::npos) return "text/plain";

//   std::string ext = path.substr(dotPos);
  
//   if (ext == ".html" || ext == ".htm") return "text/html";
//   if (ext == ".css") return "text/css";
//   if (ext == ".js") return "application/javascript";
//   if (ext == ".png") return "image/png";
//   if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
//   if (ext == ".gif") return "image/gif";
//   if (ext == ".ico") return "image/x-icon";
//   if (ext == ".pdf") return "application/pdf";
//   if (ext == ".txt") return "text/plain";

//   return "application/octet-stream";
// }