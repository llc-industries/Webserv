#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class HttpRequest {
public:
  HttpRequest();
  ~HttpRequest();
  // recv()
  void swallow(const char *buffer, size_t bytes);
  // getters
  bool isComplete() const;
  std::string getMethod() const;
  std::string getPath() const;
  std::string getHeader(const std::string &key) const;
  const std::string &getBody() const;
  const std::map<std::string, std::string>& getCookies() const {return _cookies;}

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
  bool _is_chunked;
  size_t _body_pos;
  size_t _content_length;
  std::map<std::string, std::string> _cookies;
  // ft de parsing de requete
  void parseFirstLine(const std::string &line);
  void parseHeaderLine(const std::string &line);
  void parseChunkedBody();
};

#endif /* HTTPREQUEST_H */
