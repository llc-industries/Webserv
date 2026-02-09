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

/* ----- HELPERS ----- */

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

void ConfigParser::_parse() {
  while (_current < _tokens.size())
    _parseServerBlock();
}

void ConfigParser::_advance() {
  if (_current > _tokens.size())
    throw std::runtime_error("_advance too far"); // TODO Format error
  _current++;
  return;
}

void ConfigParser::_expect(std::string expected) {
  if (_current < _tokens.size() && expected == _tokens[_current].tok) {
    this->_advance();
    return;
  }

  std::stringstream ss;

  ss << _configPath << " (line: " << _tokens[_current].line
     << ") expected: " << expected << " but got " << _tokens[_current].tok;

  // TODO: Separate EOF and expected != tok errors

  throw std::runtime_error(ss.str());
}

/* ----- ACTUAL PARSER ----- */

void ConfigParser::_parseServerBlock() {
  _expect("server");
  _expect("{");

  ServerConfig sc;

  // TODO: Dispatch func
  while (_tokens[_current].tok != "}") {
  }
}

void ConfigParser::_parseLocationBlock() { return; }
