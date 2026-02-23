#include "ConfigTokenizer.hpp"
#include "logs.hpp"

/* Constructor */

ConfigTokenizer::ConfigTokenizer(const std::string &configPath,
                                 std::vector<Token> &tokens)
    : _tokens(tokens) {
  LOG_INFO("Loading " << configPath << "...");
  this->_loadFile(configPath);
  LOG_INFO("Config file loaded into memory");

  LOG_INFO("Starting tokenization...");
  this->_tokenize();
  if (tokens.empty())
    throw std::runtime_error(configPath + " is empty");
  LOG_INFO("Tokenisation done. " << _tokens.size() << " tokens created");
}

/* Methods */

void ConfigTokenizer::_loadFile(const std::string &configPath) {
  std::ifstream file(configPath.c_str());
  if (file.is_open() == false)
    throw std::runtime_error("Error while opening " + configPath + ": " +
                             strerror(errno));

  std::string line;
  while (std::getline(file, line)) {
    this->_fileContent += line;
    this->_fileContent += '\n';
  }

  // Throw if read error
  if (file.bad() == true || file.eof() == false)
    throw std::runtime_error("Error while reading " + configPath);
  if (this->_fileContent.empty() == true)
    throw std::runtime_error(configPath + " is empty");
}

void ConfigTokenizer::_tokenize() {

  std::string buf;
  size_t curLine = 1;
  size_t curCol = 0;
  size_t startCol = 0;

  for (size_t i = 0; i < this->_fileContent.length(); i++) {
    char c = this->_fileContent[i];
    curCol++;

    if (c == '\n') {
      curLine++;
      curCol = 0;
      continue;
    }

    if (c == '#') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine, startCol));
        buf.clear();
      }
      while (i < this->_fileContent.length() && this->_fileContent[i] != '\n')
        i++;
      i--;
      continue;
    }

    if (std::isspace(c)) {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine, startCol));
        buf.clear();
      }
      continue;
    }

    if (c == '{' || c == '}' || c == ';') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine, startCol));
        buf.clear();
      }
      this->_tokens.push_back(Token(std::string(1, c), curLine, curCol));
      continue;
    }

    if (buf.empty() == true)
      startCol = curCol;

    buf += c;
  }
  if (buf.empty() == false)
    this->_tokens.push_back(Token(buf, curLine, startCol));
}
