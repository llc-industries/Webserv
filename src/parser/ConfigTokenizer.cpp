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

  // std::cout << this->_fileContent; /* Dump file to stdout */
}

void ConfigTokenizer::_tokenize() {

  std::string buf;
  size_t curLine = 1;

  for (size_t i = 0; i < this->_fileContent.length(); i++) {
    char c = this->_fileContent[i];

    if (c == '\n') {
      curLine++;
      continue;
    }

    if (c == '#') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine));
        buf.clear();
      }
      while (i < this->_fileContent.length() && this->_fileContent[i] != '\n')
        i++;
      i--;
      continue;
    }

    if (std::isspace(c)) {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine));
        buf.clear();
      }
      continue;
    }

    if (c == '{' || c == '}' || c == ';') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, curLine));
        buf.clear();
      }
      this->_tokens.push_back(Token(std::string(1, c), curLine));
      continue;
    }

    buf += c;
  }
  if (buf.empty() == false)
    this->_tokens.push_back(Token(buf, curLine));
  // for (std::vector<Token>::iterator it = _tokens.begin(); it !=
  // _tokens.end();
  //      ++it) {
  //   std::cout << it->tok << '\n';
  //} /* Dump tokens to stdout */
}
