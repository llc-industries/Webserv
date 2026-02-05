#ifndef CONFIG_TOKENIZER_HPP
#define CONFIG_TOKENIZER_HPP

#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h> // strerror()
#include <string>
#include <vector>

struct Token {
  std::string tok;
  size_t line;

  Token(std::string token, size_t line) : tok(token), line(line) {}
};

class ConfigTokenizer {
public:
  ConfigTokenizer(const std::string &configPath, std::vector<Token> &tokens);
  ~ConfigTokenizer() {}

private:
  const std::string &_configPath;
  std::string _fileContent;
  std::vector<Token> &_tokens;

  void _loadFile();
  void _tokenize();
};

#endif /* CONFIG_TOKENIZER_HPP */
