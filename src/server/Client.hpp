#ifndef CLIENT_H
#define CLIENT_H

#include "ConfigStructs.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <fstream>

class Client {
public:
  Client(const ServerConfig *context);
  ~Client();

  void swallow(const char *buf, ssize_t bytesRead);

  const char *getResponse() const;
  size_t getResponseLength() const;
  bool isRequestComplete() const;
  bool isResponseReady() const;

  void buildResponse();

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

  int _validateRequest() const;
  Route _resolveRoute() const;
  bool _isMethodAllowed(const Route &route) const;
  void _setError(int code);

  void _handleGet(const Route &route);
  void _handlePost(const Route &route);
  void _handleDelete(const Route &route);
};

#endif /* CLIENT_H */
