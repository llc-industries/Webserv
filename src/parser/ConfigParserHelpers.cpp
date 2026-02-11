#include "ConfigParser.hpp"
#include "logs.hpp"

/* Check duplicates value at each functions, yes...  */

void ConfigParser::_parsePort(ServerConfig &sc) {
  if (sc.port != -1)
    _parserThrowDup("listen", "server");

  std::string valueStr = _getTokStr();
  char *endptr = NULL;
  long port = std::strtol(valueStr.c_str(), &endptr, 10);

  if (valueStr.empty() || *endptr != '\0')
    _parserThrow("Invalid listen directive: " + valueStr);
  else if (port < 0 || port > 65535)
    _parserThrow("Port is out of range (0-65535): " + valueStr);

  sc.port = static_cast<int>(port);

  _advance();
  _expect(";");
}

void ConfigParser::_parseBodySize(ServerConfig &sc) {
  if (sc.maxBodySize != -1)
    _parserThrowDup("client_max_body_size", "server");

  std::string valueStr = _getTokStr();
  char *endptr = NULL;
  long bodySize = std::strtol(valueStr.c_str(), &endptr, 10);

  if (valueStr.empty() || *endptr != '\0')
    _parserThrow("Invalid client_max_body_size directive: " + valueStr);
  if (bodySize < 0)
    _parserThrow("client_max_body_size can't be negative: " + valueStr);
  if (bodySize > INT_MAX)
    _parserThrow("client_max_body_size is too big (max 2147483647), got " +
                 valueStr);

  sc.maxBodySize = static_cast<int>(bodySize);

  _advance();
  _expect(";");
}

void ConfigParser::_parseHost(ServerConfig &sc) {
  if (sc.host.empty() == false)
    _parserThrowDup("host", "server");

  std::string host = _getTokStr();
  if (host == "localhost")
    host = "127.0.0.1";

  const char *curNb = host.c_str();
  char *endptr = NULL;

  for (int i = 0; i < 4; i++) {
    long byte = std::strtol(curNb, &endptr, 10);

    if (curNb == endptr)
      _parserThrow("Invalid host format (empty block): " + host);
    if (byte < 0 || byte > 255)
      _parserThrow("Invalid host format (Out of range (0 - 255)): " + host);
    if (i < 3) {
      if (*endptr != '.')
        _parserThrow("Invalid host format (expected . separator): " + host);
      curNb = endptr + 1;
    } else {
      if (*endptr != '\0')
        _parserThrow("Invalid host format (trailing characters): " + host);
    }
  }

  sc.host = host;
  _advance();
  _expect(";");
}

void ConfigParser::_parseServerName(ServerConfig &sc) {}

void ConfigParser::_parseRoot(ServerConfig &sc) {}

void ConfigParser::_parseIndex(ServerConfig &sc) {}

void ConfigParser::_parseErrorPages(ServerConfig &sc) {}
