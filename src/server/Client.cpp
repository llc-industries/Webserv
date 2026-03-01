#include "Client.hpp"

Client::Client(const ServerConfig *context)
    : _context(context), _isRespReady(false), _isReqComplete(false) {}

Client::~Client() {}

void Client::swallow(const char *buf, ssize_t bytesRead) {
  _request.swallow(buf, bytesRead);
  _isReqComplete = _request.isComplete();
}

void Client::buildResponse() { if () }

const char *Client::getResponse() const { return _rawResponse.c_str(); }
size_t Client::getResponseLength() const { return _rawResponse.length(); }

bool Client::isRequestComplete() const { return _isReqComplete; }
bool Client::isResponseReady() const { return _isRespReady; }
