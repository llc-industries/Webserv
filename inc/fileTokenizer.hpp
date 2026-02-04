#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <errno.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

class fileTokenizer {
public:
  fileTokenizer(const std::string &filepath, std::vector<std::string> &tokens);
  ~fileTokenizer() {}

private:
  const std::string &_filePath;
  std::string _fileContent;
  std::vector<std::string> &_tokens;

  void _loadFile();
  void _tokenize();
};

#endif /* FILE_HANDLER_HPP */
