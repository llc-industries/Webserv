#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "ConfigStructs.hpp"
#include "ConfigTokenizer.hpp"

class ConfigParser {
public:
  ConfigParser(const std::string &configPath);
  ~ConfigParser() {}

  const std::vector<ServerConfig> &getConfig() const { return _config; }

private:
  std::vector<ServerConfig> _config;
  std::vector<Token> _tokens;

  void _parse();
};

#endif /* CONFIG_PARSER_HPP */
