#include "ConfigTokenizer.hpp"
#include "logs.hpp"

/* Constructor */

ConfigTokenizer::ConfigTokenizer(const std::string &configPath,
                                 std::vector<Token> &tokens)
    : _configPath(configPath), _tokens(tokens) {
  this->_loadFile();
  this->_tokenize();
}

/* Methods */

void ConfigTokenizer::_loadFile() {
  LOG_INFO("Loading " << this->_configPath << "...");

  std::ifstream file(this->_configPath.c_str());
  if (file.is_open() == false)
    throw std::runtime_error("Error while opening " + this->_configPath + ": " +
                             strerror(errno));

  std::string line;
  while (std::getline(file, line)) {
    this->_fileContent += line;
    this->_fileContent += '\n';
  }

  // Throw if read error
  if (file.bad() == true || file.eof() == false)
    throw std::runtime_error("Error while reading " + this->_configPath);
  if (this->_fileContent.empty() == true)
    throw std::runtime_error(this->_configPath + " is empty");

  LOG_INFO("Config file loaded into memory");
  // std::cout << this->_fileContent; /* Dump file to stdout */
}

void ConfigTokenizer::_tokenize() {
  LOG_INFO("Starting tokenization...");

  std::string buf;
  size_t cur_line = 1;

  for (size_t i = 0; i < this->_fileContent.length(); i++) {
    char c = this->_fileContent[i];

    // 1 - newline
    if (c == '\n') {
      cur_line++;
      continue;
    }

    // 2 - comments (flush puis ignore)
    if (c == '#') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, cur_line));
        buf.clear();
      }
      while (this->_fileContent[i] && this->_fileContent[i] != '\n')
        i++;
      i--;
      continue;
    }

    // 3 - Spaces
    if (std::isspace(c)) {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, cur_line));
        buf.clear();
      }
      continue;
    }

    // 4 - {} ;
    if (c == '{' || c == '}' || c == ';') {
      if (buf.empty() == false) {
        this->_tokens.push_back(Token(buf, cur_line));
        buf.clear();
      }
      this->_tokens.push_back(Token(std::string(1, c), cur_line));
      continue;
    }

    // 5 - Flush char +=
    buf += c;
  }
  if (buf.empty() == false)
    this->_tokens.push_back(Token(buf, cur_line));

  LOG_INFO("Tokenisation done");
  for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end();
       ++it) {
    std::cout << it->tok << '\n';
  }
}
