#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "ConfigStructs.hpp"
#include "ConfigTokenizer.hpp"
#include <map>
#include <sstream>

class ConfigParser {
public:
  ConfigParser(const std::string &configPath);
  ~ConfigParser() {}

  const std::vector<ServerConfig> &getConfig() const { return _config; }

private:
  std::vector<ServerConfig> _config;
  const std::string &_configPath;

  size_t _current; // Current token idx
  std::vector<Token> _tokens;

  typedef void (ConfigParser::*servFunc)(ServerConfig &);
  typedef void (ConfigParser::*LocFunc)(Location &);
  std::map<std::string, servFunc> _serverFuncTable;
  std::map<std::string, LocFunc> _locFuncTable;

  void _initFuncTables();

  void _parse();
  void _advance();
  void _expect(std::string expected);
  void _parseServerBlock();
  void _parseLocationBlock();

  // Defined in ConfigParserHelpers.cpp
  void _parsePort(ServerConfig &sc);
  void _parseBodySize(ServerConfig &sc);
  void _parseHost(ServerConfig &sc);
  void _parseServerName(ServerConfig &sc);
  void _parseRoot(ServerConfig &sc);
  void _parseIndex(ServerConfig &sc);
  void _parseErrorPages(ServerConfig &sc);
};

#endif /* CONFIG_PARSER_HPP */
