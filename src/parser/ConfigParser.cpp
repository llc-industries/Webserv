#include "ConfigParser.hpp"
#include "logs.hpp"

typedef std::vector<ServerConfig>::iterator it_config;
typedef std::vector<Location>::iterator it_loc;

/* ----- CONSTRUCTOR ----- */

ConfigParser::ConfigParser(const std::string &configPath)
    : _configPath(configPath), _current(0) {

  ConfigTokenizer(configPath, this->_tokens);

  LOG_INFO("Starting parser...");
  this->_initFuncTables();
  this->_parse();
  this->_checkDefaults();
  LOG_INFO("Parser done");
}

/* ----- TOKEN HELPERS ----- */

const std::string &ConfigParser::_getTokStr() const {
  if (_current < _tokens.size())
    return _tokens[_current].tok;
  return _tokens.back().tok;
}

size_t ConfigParser::_getTokLine() const {
  if (_current < _tokens.size())
    return _tokens[_current].line;
  return _tokens.back().line;
}

size_t ConfigParser::_getTokCol() const {
  if (_current < _tokens.size())
    return _tokens[_current].col;
  return _tokens.back().col;
}

/* ----- FUNC TABLE INIT ----- */

void ConfigParser::_initFuncTables() {
  _serverFuncTable["listen"] = &ConfigParser::_parsePorts;
  _serverFuncTable["host"] = &ConfigParser::_parseHost;
  _serverFuncTable["server_name"] = &ConfigParser::_parseServerName;
  _serverFuncTable["error_page"] = &ConfigParser::_parseErrorPages;
  _serverFuncTable["client_max_body_size"] = &ConfigParser::_parseBodySize;
  _serverFuncTable["root"] = &ConfigParser::_parseRoot;
  _serverFuncTable["index"] = &ConfigParser::_parseIndex;

  _locFuncTable["allow_methods"] = &ConfigParser::_parseLocMethods;
  _locFuncTable["return"] = &ConfigParser::_parseLocRet;
  _locFuncTable["cgi_pass"] = &ConfigParser::_parseLocCgiPath;
  _locFuncTable["upload_store"] = &ConfigParser::_parseLocPost;
  _locFuncTable["autoindex"] = &ConfigParser::_parseLocAutoIndex;
  _locFuncTable["root"] = &ConfigParser::_parseLocRoot;
  _locFuncTable["index"] = &ConfigParser::_parseLocIndex;
}

/* ----- PARSER LOGIC ----- */

void ConfigParser::_parse() {
  while (_current < _tokens.size())
    _parseServerBlock();
}

void ConfigParser::_advance() {
  if (_current >= _tokens.size())
    _parserThrow("Unexpected EOF after " + _getTokStr());
  _current++;
}

void ConfigParser::_expect(const std::string &expected) {
  if (_current < _tokens.size() && expected == _getTokStr()) {
    this->_advance();
    return;
  }

  if (_current < _tokens.size())
    _parserThrow("expected " + expected + " but got " + _getTokStr());
  else
    _parserThrow("expected " + expected + " but reached end of file");
}

void ConfigParser::_parserThrow(const std::string &error) {
  std::stringstream ss;
  ss << RED "(" << _configPath << ':' << _getTokLine() << ':' << _getTokCol()
     << ") -> " RESET << error;

  throw std::runtime_error(ss.str());
}

void ConfigParser::_parserThrowDup(const std::string &directive,
                                   const std::string &context) {
  std::stringstream ss;
  ss << RED "(" << _configPath << ':' << _getTokLine() << ") -> " RESET
     << "Duplicate " << directive << " directive in " << context << " block";

  throw std::runtime_error(ss.str());
}

void ConfigParser::_parseServerBlock() {
  _expect("server");
  _expect("{");

  ServerConfig sc;

  while (_current < _tokens.size() && _getTokStr() != "}") {
    std::string token = _getTokStr();

    if (_serverFuncTable.count(token)) {
      servFunc func = _serverFuncTable[token];
      _advance();
      if (_getTokStr() == ";")
        _parserThrow(token + " directive can't be empty");
      (this->*func)(sc);
    } else if (token == "location") {
      _parseLocationBlock(sc);
    } else
      _parserThrow("Unknown directive: " + token);
  }

  _expect("}");
  _config.push_back(sc);
}

void ConfigParser::_parseLocationBlock(ServerConfig &sc) {
  Location loc;

  _advance();
  _parseLocPath(loc);
  _expect("{");

  while (_current < _tokens.size() && _getTokStr() != "}") {
    std::string token = _getTokStr();

    if (_locFuncTable.count(token)) {
      LocFunc func = _locFuncTable[token];
      _advance();
      if (_getTokStr() == ";")
        _parserThrow(token + " directive can't be empty");
      (this->*func)(loc);
    } else
      _parserThrow("Unknown directive in location block: " + token);
  }

  _expect("}");
  sc.locations.push_back(loc);
}

void ConfigParser::_checkDefaults() {
  std::vector<int> allPorts;

  for (size_t i = 0; i < _config.size(); i++) {
    ServerConfig &sc = _config[i];
    std::string block(1, i + 49); // Ugly

    if (sc.host.empty()) {
      LOG_WARN("No host value in server block " + block +
               "... Using localhost as default");
      sc.host = "127.0.0.1";
    }
    if (sc.root.empty())
      throw std::runtime_error("No root value in server block " + block);
    if (sc.index.empty()) {
      LOG_WARN("No index value in server block " + block +
               "... Using index.html as default");
      sc.index.push_back("index.html");
    }
    if (sc.maxBodySize == -1) {
      LOG_WARN("No max body size for server block " + block +
               "... Using 1MB as default");
      sc.maxBodySize = 1000000;
    }

    for (size_t k = 0; k < sc.ports.size(); k++) {
      int port = sc.ports[k];
      if (std::find(allPorts.begin(), allPorts.end(), port) != allPorts.end()) {
        std::stringstream ss;
        ss << "Duplicate port " << port << " in server block " << block;
        throw std::runtime_error(ss.str());
      }
      allPorts.push_back(port);
    }

    for (size_t j = 0; j < _config[i].locations.size(); j++) {
      Location &loc = sc.locations[j];

      if (loc.root.empty())
        loc.root = sc.root;
      if (loc.index.empty())
        loc.index = sc.index;
    }
  }
}
