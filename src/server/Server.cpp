#include "Server.hpp"

Server::Server(const std::vector<ServerConfig> &config) : _config(config) {
  struct addrinfo hints;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  std::memset(&hints, 0, sizeof(addrinfo));
}
Server::~Server() {}

void Server::run() {}
