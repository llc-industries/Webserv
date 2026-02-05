#include "ConfigParser.hpp"
#include "logs.hpp"

ConfigParser::ConfigParser(const std::string &configPath) {
  ConfigTokenizer(configPath, this->_tokens);
}
