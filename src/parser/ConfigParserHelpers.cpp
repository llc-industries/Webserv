#include "ConfigParser.hpp"
#include "logs.hpp"

/* -------- Server functions -------- */

void ConfigParser::_parsePort(ServerConfig &sc) {
  if (sc.port != -1)
    _parserThrowDup("listen", "server");

  std::string valueStr = _getTokStr();
  char *endptr = NULL;
  long port = std::strtol(valueStr.c_str(), &endptr, 10);

  if (valueStr.empty() || *endptr != '\0')
    _parserThrow("Invalid listen directive: " + valueStr);
  else if (port < 0 || port > 65535)
    _parserThrow("Port " + valueStr + " is out of range (0-65535)");

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
      _parserThrow("Invalid host format (out of range (0 - 255)): " + host);
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

void ConfigParser::_parseServerName(ServerConfig &sc) {
  if (sc.serverName.empty() == false)
    _parserThrowDup("server_name", "server");

  sc.serverName = _getTokStr();

  _advance();
  _expect(";");
}

void ConfigParser::_parseRoot(ServerConfig &sc) {
  if (sc.root.empty() == false)
    _parserThrowDup("root", "server");

  sc.root = _getTokStr();

  _advance();
  _expect(";");
}

void ConfigParser::_parseIndex(ServerConfig &sc) {
  while (_current < _tokens.size() && _getTokStr() != ";") {
    sc.index.push_back(_getTokStr());
    _advance();
  }

  _expect(";");
}

void ConfigParser::_parseErrorPages(ServerConfig &sc) {
  std::vector<std::string> args;

  while (_current < _tokens.size() && _getTokStr() != ";") {
    args.push_back(_getTokStr());
    _advance();
  }

  if (args.size() < 2)
    _parserThrow("error_pages requires 2 arguments (error - page)");

  std::string url = args.back();
  args.pop_back();

  for (size_t i = 0; i < args.size(); i++) {
    char *endptr = NULL;
    long code = std::strtol(args[i].c_str(), &endptr, 10);

    if (*endptr != '\0')
      _parserThrow("Invalid error code " + args[i]);
    if (code < 300 || code > 599)
      _parserThrow("Error code " + args[i] + " is out of range (300 - 599) ");

    sc.errPages[static_cast<int>(code)] = url;
  }

  _expect(";");
}

/* -------- Location functions -------- */

void ConfigParser::_parseLocPath(Location &loc) {
  if (_getTokStr() == "{" || _getTokStr() == ";")
    _parserThrow("location block needs a pattern");

  loc.path = _getTokStr();
  _advance();
}

void ConfigParser::_parseLocMethods(Location &loc) {
  if (loc.methods.empty() == false)
    _parserThrowDup("allow_methods", "location");

  while (_current < _tokens.size() && _getTokStr() != ";") {
    std::string arg = _getTokStr();
    if (arg != "GET" && arg != "POST" && arg != "DELETE")
      _parserThrow("Unknown argument in allow_methods directive " + arg);

    if (std::find(loc.methods.begin(), loc.methods.end(), arg) ==
        loc.methods.end())
      loc.methods.push_back(arg);

    _advance();
  }

  _expect(";");
}

/* void ConfigParser::_parseLocRet(Location &loc) {
  if (loc.ret.first != 0)
    _parserThrowDup("return", "location");

  _expect(";");
}

void ConfigParser::_parseLocCgiPath(Location &loc) {
  if (loc.cgiPath.empty() == false)
    _parserThrowDup("cgi_pass", "location");

  _expect(";");
}

void ConfigParser::_parseLocPost(Location &loc) {
  if (loc.postPath.empty() == false)
    _parserThrowDup("", "location");

  _expect(";");
}

void ConfigParser::_parseLocAutoIndex(Location &loc) {
  if (loc..empty() == false)
    _parserThrowDup("", "location");

  _expect(";");
}

void ConfigParser::_parseLocRoot(Location &loc) {
  if (loc.root.empty() == false)
    _parserThrowDup("root", "location");

  loc.root = _getTokStr();

  _advance();
  _expect(";");
}

void ConfigParser::_parseLocIndex(Location &loc) {
  if (loc..empty() == false)
    _parserThrowDup("", "location");

  _expect(";");
} */
