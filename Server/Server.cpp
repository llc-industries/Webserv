/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 15:34:07 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/19 22:52:37 by atazzit          ###   ########.fr       */
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
    std::cerr << "Epoll creation fail\n";
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < ports.size(); i++){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
      std::cerr << "Socket creation failed\n";
      continue;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
      std::cerr << "Socket setting option failed\n";
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(ports[i]);
    address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0) {
      std::cout << "bind failed at port" << ports[i] << "\n";
      close(server_fd);
      continue;
    }
    if (listen(server_fd, SOMAXCONN) < 0) {
      std::cout << "listening failed\n";
      close(server_fd);
      continue;
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
      std::cout << "epoll ctl bind failed\n";
      close(server_fd);
      continue;
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
    buffer[bytes] = '\0';
    _clients[client_fd].request.append(buffer, bytes);
    ClientData &client = _clients[client_fd];
    if (!client.header_parsed)
    { 
      size_t header_end = client.request.find("\r\n\r\n");
      if (header_end != std::string::npos)
        parseRequestHeader(client_fd);
    }
    if (client.header_parsed)
    {
      size_t body_start = client.request.find("\r\n\r\n") + 4;
      size_t current_body_size = client.request.size() - body_start;
      size_t expected_size = client.content_length;
     
      if (current_body_size >= expected_size)
      {
      
        epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD ,client_fd, &ev);
      }
    }  
  
}

void Server::closeClient(int client_fd)
{
  epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
  close(client_fd);
  std::cout << "[INFO] Connection closed for fd:" << client_fd << "\n";
}

void Server::sendResponse(int client_fd)
{
  ClientData &client = _clients[client_fd];
  //logique POST
  if (client.method == "POST"){
    size_t body_start = client.request.find("\r\n\r\n");
    if (body_start != std::string::npos){
        body_start += 4;
        std::string body_content = client.request.substr(body_start);
        std::string save_path = "./www/uploaded_file.txt";
        std::ofstream outfile(save_path.c_str(), std::ios::binary);
        std::ostringstream response;
        if (outfile.is_open())
        {
          outfile.write(body_content.c_str(), body_content.size());
          outfile.close();
          std::string response_body = "<html><body><h1>Upload réussi ! Fichier sauvegarde.</h1></body></html>";
          response << "HTTP/1.1 201 Created\r\n";
          response << "Content-Type: text/html\r\n";
          response << "Content-Length: " << response_body.size() << "\r\n";
          response << "Connection: close\r\n\r\n";
          response << response_body;
        }
        else {
          std::string error_body = "<html><body><h1>500 Internal Server Error</h1></body></html>";
              response << "HTTP/1.1 500 Internal Server Error\r\n";
              response << "Content-Type: text/html\r\n";
              response << "Content-Length: " << error_body.size() << "\r\n";
              response << "Connection: close\r\n\r\n";
              response << error_body;
          }
          send(client_fd, response.str().c_str(), response.str().size(), 0);
      }
      closeClient(client_fd);
      return;
  }
  
  //logic GET
  std::string root = "./www";
  std::string full_path = root + client.path;

  if (client.path == "/")
    full_path = root + "/index.html";
  
  std::ifstream file(full_path.c_str(), std::ios::binary);
  
  if (!file.is_open())
  {
    std::string body = "<html><body><h1> 404 Not Found </h1></body></html>";
    std::ostringstream oss;
    oss << "HTTP/1.1 404 Not Found\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;
    send(client_fd, oss.str().c_str(), oss.str().size(), 0);
  }
  else
  {
    std::ostringstream file_content;
    file_content << file.rdbuf();
    std::string body = file_content.str();
    std::ostringstream header;
    header << "HTTP/1.1 200 OK\r\n";
    header << "Content-Type: "<< getContentType(full_path) << "\r\n";
    header << "Content-Length: " << body.size() << "\r\n";
    header << "Connection: close\r\n\r\n"; 
    std::string full_res = header.str() + body;
    send(client_fd, full_res.c_str(), full_res.size(), 0);
  }
  closeClient(client_fd);
}

void Server::parseRequestHeader(int client_fd)
{
  ClientData &Client = _clients[client_fd];
  std::stringstream ss(Client.request);
  std::string line;
  if (getline(ss, line))
  {
    std::stringstream first_line(line);
    first_line >> Client.method;
    first_line >> Client.path;
  }
  while (getline(ss, line) && line != "\r" && line != ""){
    if (line.find("Host: ") == 0){
      Client.host = line.substr(6);
      size_t pos = Client.host.find("\r");
      if (pos != std::string::npos) Client.host.erase(pos);
    }
    if (line.find("Content-Length: ") == 0) {
        Client.content_length = std::atoi(line.substr(16).c_str());
    }
  }
  Client.header_parsed = true;
  std::cout << "[PARSED] Method: " << Client.method << " Path: " << Client.path << std::endl;
}

std::string Server::getContentType(const std::string& path) {
  size_t dotPos = path.find_last_of(".");
  if (dotPos == std::string::npos) return "text/plain";

  std::string ext = path.substr(dotPos);
  //TODO: utiliser des map apres si g pas la flemmme
  if (ext == ".html" || ext == ".htm") return "text/html";
  if (ext == ".css") return "text/css";
  if (ext == ".js") return "application/javascript";
  if (ext == ".png") return "image/png";
  if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
  if (ext == ".gif") return "image/gif";
  if (ext == "ico") return "image/x-icon";
  if (ext == ".pdf") return "application/pdf";
  if (ext == ".txt") return "text/plain";

  return "application/octet-stream";
}