#include "ConfigParser.hpp"
#include "logs.hpp"

void ConfigParser::_parsePort(ServerConfig &sc) {
  std::string valueStr = _getTokStr();

  char *endptr = NULL;

  long port = std::strtol(valueStr.c_str(), &endptr, 10);

  if (valueStr.empty() || *endptr != '\0')
    _parserThrow("Invalid listen directive: " + valueStr);
  else if (port < 0 || port > 65535)
    _parserThrow("Port is out of range (0-65535): " + valueStr);

  sc.port = port;

  _advance();
  _expect(";");
}

void ConfigParser::_parseBodySize(ServerConfig &sc) {}

void ConfigParser::_parseHost(ServerConfig &sc) {}

void ConfigParser::_parseServerName(ServerConfig &sc) {}

void ConfigParser::_parseRoot(ServerConfig &sc) {}

void ConfigParser::_parseIndex(ServerConfig &sc) {}

void ConfigParser::_parseErrorPages(ServerConfig &sc) {}
