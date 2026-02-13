#ifndef CONFIG_TOKENIZER_HPP
#define CONFIG_TOKENIZER_HPP

#include <cerrno>
#include <fstream>
#include <iostream>
#include <string.h> // strerror()
#include <string>
#include <vector>

struct Token {
  std::string tok;
  size_t line;
  size_t col;

  Token(std::string token, size_t line, size_t col)
      : tok(token), line(line), col(col) {}
};

class ConfigTokenizer {
public:
  ConfigTokenizer(const std::string &configPath, std::vector<Token> &tokens);
  ~ConfigTokenizer() {}

private:
  std::string _fileContent;
  std::vector<Token> &_tokens;

  void _loadFile(const std::string &configPath);
  void _tokenize();
};

#endif /* CONFIG_TOKENIZER_HPP */
