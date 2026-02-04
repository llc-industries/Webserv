#ifndef FILE_TOKENIZER_HPP
#define FILE_TOKENIZER_HPP

#include <errno.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

struct Token {
  std::string tok;
  size_t line;

  Token(std::string token, size_t line) : tok(token), line(line) {}
};

class fileTokenizer {
public:
  fileTokenizer(const std::string &filepath, std::vector<Token> &tokens);
  ~fileTokenizer() {}

private:
  const std::string &_filePath;
  std::string _fileContent;
  std::vector<Token> &_tokens;

  void _loadFile();
  void _tokenize();
};

#endif /* FILE_TOKENIZER_HPP */
