#include "ConfigParser.hpp"
#include "logs.hpp"

void ConfigParser::_parsePort(ServerConfig &sc) {

  char *endptr = NULL;
  int port = std::strtol(_getTokStr().c_str(), &endptr, 10);

  _advance();
  _expect(";");
}

void ConfigParser::_parseBodySize(ServerConfig &sc) {}

void ConfigParser::_parseHost(ServerConfig &sc) {}

void ConfigParser::_parseServerName(ServerConfig &sc) {}

void ConfigParser::_parseRoot(ServerConfig &sc) {}

void ConfigParser::_parseIndex(ServerConfig &sc) {}

void ConfigParser::_parseErrorPages(ServerConfig &sc) {}
