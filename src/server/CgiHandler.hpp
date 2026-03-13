#ifndef CGIHANDLER_H
#define CGIHANDLER_H

#include "HttpRequest.hpp"
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

class CgiHandler {
public:
  CgiHandler(const HttpRequest &request, const std::string &scriptPath,
             const std::string &cgiBinPath);
  ~CgiHandler();

  int executeCgi(pid_t &out_pid, int &out_fd, int &in_fd); // execute le cgi
private:
  const HttpRequest &_request;
  std::string _scriptPath;
  std::string _cgiBinPath;
  std::map<std::string, std::string> _envMap;

  void _initEnv(); // initialise les variables d'env
  char **_getEnvArray() const;
};

#endif /* CGIHANDLER_H */
