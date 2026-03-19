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

  void swallow(const char *buffer, size_t bytes, int maxBodySize);
  bool isComplete() const;
  std::string getMethod() const;
  std::string getPath() const;
  std::string getHeader(const std::string &key) const;
  int getErrorCode() const;
  const std::string &getBody() const;
  const std::map<std::string, std::string> &getCookies() const {
    return _cookies;
  }

private:
  std::string _raw_data;
  // Attributes
  std::string _method;
  std::string _path;
  std::string _version;
  std::map<std::string, std::string> _headers;
  std::string _body;
  // State
  bool _headers_parsed;
  bool _is_complete;
  bool _is_chunked;
  size_t _body_pos;
  size_t _content_length;
  std::map<std::string, std::string> _cookies;
  int _error_code;
  void parseFirstLine(const std::string &line);
  void parseHeaderLine(const std::string &line);
  void parseChunkedBody(int maxBodySize);
};

#endif /* HTTPREQUEST_H */
