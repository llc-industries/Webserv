#include "ConfigParser.hpp"
#include "logs.hpp"

ConfigParser::ConfigParser(const std::string &configPath) {
  ConfigTokenizer(configPath, this->_tokens);

  LOG_INFO("Starting parser...");
  this->_parse();
  LOG_INFO("Parser done");
}

void ConfigParser::_parse() { return; }

void ConfigParser::_advance() { return; }

void ConfigParser::_expect() { return; }

void ConfigParser::_parseServerBlock() { return; }

void ConfigParser::_parseLocationBlock() { return; }
