#ifndef CLIENT_H
#define CLIENT_H

#include "ConfigStructs.hpp"

class Client {
public:
  Client(const ServerConfig *context);
  ~Client();

private:
  const ServerConfig *_context;
};

#endif /* CLIENT_H */
