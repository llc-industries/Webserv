#ifndef CLIENT_H
#define CLIENT_H

#include "ConfigStructs.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client {
public:
  Client(const ServerConfig *context);
  ~Client();

  void swallow(const char *buf, ssize_t bytesRead);
  bool isRequestComplete() const;

  void buildResponse();
  bool isResponseReady() const;

  const char *getResponse() const;
  size_t getResponseLength() const;

private:
  const ServerConfig *_context;

  Request _request;
  bool _isReqComplete;

  Response _response;
  std::string _rawResponse;
  bool _isRespReady;
};

#endif /* CLIENT_H */
