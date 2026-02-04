#include "fileTokenizer.hpp"
#include "debug.hpp"

/* Constructor */

fileTokenizer::fileTokenizer(const std::string &filepath,
                             std::vector<Token> &tokens)
    : _filePath(filepath), _tokens(tokens) {
  this->_loadFile();
  this->_tokenize();
}

/* Methods */

void fileTokenizer::_loadFile() {
  LOG_INFO("Loading " << this->_filePath << "...");

  std::ifstream file(this->_filePath.c_str());
  if (file.is_open() == false)
    throw std::runtime_error("Error while opening " + this->_filePath + ": " +
                             strerror(errno));

  std::string line;
  while (std::getline(file, line)) {
    this->_fileContent += line;
    this->_fileContent += '\n';
  }

  // Throw if read error
  if (file.bad() == true || file.eof() == false)
    throw std::runtime_error("Error while reading " + this->_filePath);
  if (this->_fileContent.empty() == true)
    throw std::runtime_error(this->_filePath + " is empty");

  LOG_INFO("Config file loaded into memory");
  // std::cout << this->_fileContent; /* Dump file to stdout */
}

void fileTokenizer::_tokenize() {
  LOG_INFO("Starting tokenization...");

  std::string buf;

  for (size_t i = 0; i < this->_fileContent.length(); i++) {
  }

  LOG_INFO("Tokenisation done");
}
