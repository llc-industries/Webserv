#include "ConfigParser.hpp"
#include "logs.hpp"

ConfigParser::ConfigParser(const std::string &configPath)
    : _configPath(configPath), _current(0) {

  ConfigTokenizer(configPath, this->_tokens);

  LOG_INFO("Starting parser...");
  this->_initFuncTables();
  this->_parse();
  LOG_INFO("Parser done");
}

/* ----- INIT ----- */

void ConfigParser::_initFuncTables() {
  _serverFuncTable["listen"] = &ConfigParser::_parsePort;
  _serverFuncTable["host"] = &ConfigParser::_parseHost;
  _serverFuncTable["server_name"] = &ConfigParser::_parseServerName;
  _serverFuncTable["error_page"] = &ConfigParser::_parseErrorPages;
  _serverFuncTable["client_max_body_size"] = &ConfigParser::_parseBodySize;
  _serverFuncTable["root"] = &ConfigParser::_parseRoot;
  _serverFuncTable["index"] = &ConfigParser::_parseIndex;

  // _locFuncTable[""] = &ConfigParser::; ...
}

/* ----- GETTERS ----- */

// Both returns the last token if _current >=
// _tokens.size()

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

/* ----- HELPERS ----- */

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

/* ----- ACTUAL PARSER ----- */

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
  /*
    _advance();
    // _parseLocationPath();
    _expect("{");
    Location loc;

    while (_getTokStr() != "}") {
      std::string token = _tokens[_current].tok;

      if (_locFuncTable.count(token)) {
        LocFunc func = _locFuncTable[token];
        (this->*func)(loc);
      } else
        _parserThrow("Unknown directive in location block: " + token);
    }

    _expect("}");
    sc.locations.push_back(loc); */
}
