#include "debug.hpp"
#include "fileTokenizer.hpp"

#include <iostream>
#include <stdlib.h>

bool handleConfig(int argc, char **argv) {
  std::vector<Token> tokens;
  std::string filepath;

  if (argc < 2) {
    LOG_WARN("No arguments provided, using default configuration file");
    LOG_INFO("Using ./webserv.conf as config file");
    filepath = "./webserv.conf";
  } else if (argc > 2) {
    LOG_WARN("Too many arguments provided");
    LOG_INFO("Using \"" << argv[1] << "\" as config file");
    filepath = argv[1];
  } else {
    LOG_INFO("Using \"" << argv[1] << "\" as config file");
    filepath = argv[1];
  }

  try {
    fileTokenizer fh(filepath, tokens);
  } catch (const std::exception &e) {
    LOG_ERR(e.what());
    return false;
  }

  return true;
}

int main(int argc, char **argv) {
  if (handleConfig(argc, argv) == false)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
