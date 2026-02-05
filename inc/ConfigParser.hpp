#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "ConfigTokenizer.hpp"

class ConfigParser {
public:
  ConfigParser(const std::string &configPath);
  ~ConfigParser() {}

private:
  std::vector<Token> _tokens;
};

#endif /* CONFIG_PARSER_HPP */
