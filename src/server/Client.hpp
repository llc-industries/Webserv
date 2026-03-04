#ifndef CLIENT_H
#define CLIENT_H

#include "ConfigStructs.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>

class Client {
public:
  Client(const ServerConfig *context);
  ~Client();

  void buildResponse();

  // In ClientUtils.cpp
  void swallow(const char *buf, ssize_t bytesRead);
  const char *getResponse() const;
  size_t getResponseLength() const;
  bool isRequestComplete() const;
  bool isResponseReady() const;

private:
  struct Route {
    const Location *loc;
    std::string root;
    std::string full_path;

    Route() : loc(NULL) {}
  };

  const ServerConfig *_context;

  HttpRequest _request;
  bool _isReqComplete;

  HttpResponse _response;
  std::string _rawResponse;
  bool _isRespReady;

  void _handleGet(const Route &route);
  void _handlePost(const Route &route);
  void _handleDelete(const Route &route);

  // In ClientUtils.cpp
  int _validateRequest() const;
  Route _resolveRoute() const;
  bool _isMethodAllowed(const Route &route) const;
  void _setError(int code);
  bool _isDir(const std::string &path) const;
  std::string _autoIndex(const std::string &path,
                         const std::string &req_path) const;
  bool _multiPart(const std::string &body, const std::string &boundary,
                  std::string &outfile, std::string &outcontent) const;
};

#endif /* CLIENT_H */
