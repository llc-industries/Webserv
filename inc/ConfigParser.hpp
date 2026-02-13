#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "ConfigStructs.hpp"
#include "ConfigTokenizer.hpp"
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <map>
#include <sstream>

#define RESET "\033[0m"
#define RED "\033[31m"

class ConfigParser {
public:
  ConfigParser(const std::string &configPath);
  ~ConfigParser() {}

  const std::vector<ServerConfig> &getConfig() const { return _config; }

private:
  // Config
  std::vector<ServerConfig> _config; // -> Parser output
  const std::string &_configPath;

  // Token helpers
  size_t _current; // Current token idx
  std::vector<Token> _tokens;
  const std::string &_getTokStr() const;
  size_t _getTokLine() const;
  size_t _getTokCol() const;

  // Parser function tables
  typedef void (ConfigParser::*servFunc)(ServerConfig &);
  typedef void (ConfigParser::*LocFunc)(Location &);
  std::map<std::string, servFunc> _serverFuncTable;
  std::map<std::string, LocFunc> _locFuncTable;
  void _initFuncTables();

  // Parser logic
  void _parse();
  void _advance();
  void _expect(const std::string &expected);
  void _parserThrow(const std::string &error);
  void _parserThrowDup(const std::string &directive,
                       const std::string &context);
  void _parseServerBlock();
  void _parseLocationBlock(ServerConfig &sc);

  // Parsing functions -- Defined in ConfigParserHelpers.cpp
  void _parsePorts(ServerConfig &sc);
  void _parseBodySize(ServerConfig &sc);
  void _parseHost(ServerConfig &sc);
  void _parseServerName(ServerConfig &sc);
  void _parseRoot(ServerConfig &sc);
  void _parseIndex(ServerConfig &sc);
  void _parseErrorPages(ServerConfig &sc);
  void _parseLocPath(Location &loc);
  void _parseLocMethods(Location &loc);
  void _parseLocRet(Location &loc);
  void _parseLocCgiPath(Location &loc);
  void _parseLocPost(Location &loc);
  void _parseLocAutoIndex(Location &loc);
  void _parseLocRoot(Location &loc);
  void _parseLocIndex(Location &loc);
};

#endif /* CONFIG_PARSER_HPP */
