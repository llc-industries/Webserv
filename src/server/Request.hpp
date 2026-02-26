#ifndef REQUEST_H
#define REQUEST_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class Request {

public:
  Request();
  ~Request();
  // recv()
  void swallow(const char *buffer, size_t bytes);
  // getters
  bool isComplete() const;
  std::string getMethod() const;
  std::string getPath() const;
  std::string getHeader(const std::string &key) const;
  std::string getBody() const;

private:
  std::string _raw_data;
  // attribue parse
  std::string _method;
  std::string _path;
  std::string _version;
  std::map<std::string, std::string> _headers;
  std::string _body;
  // etat du parsing
  bool _headers_parsed;
  bool _is_complete;
  size_t _content_length;
  // ft de parsing de requete
  void parseFirstLine(const std::string &line);
  void parseHeaderLine(const std::string &line);
};

#endif /* REQUEST_H */
