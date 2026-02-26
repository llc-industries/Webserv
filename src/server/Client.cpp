#include "Client.hpp"

Client::Client(const ServerConfig *context)
    : _context(context), _isRespReady(false), _isReqComplete(false) {}

Client::~Client() {}

void Client::swallow(const char *buf, ssize_t bytesRead) {}
bool Client::isRequestComplete() const {}

void Client::buildResponse() {}
const char *Client::getResponse() const {}
size_t Client::getResponseLength() const {}
