#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <cstdlib>
#include <map>
#include <sstream>
#include <string>

class HttpResponse {
public:
  HttpResponse();
  ~HttpResponse();

  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &body);
  // MIME
  void autoDetectContentType(const std::string &path);
  std::string toString() const;

private:
  int _status_code;
  std::string _status_message;
  std::map<std::string, std::string> _headers;
  std::string _body;

  std::string getReasonPhrase(int code) const; // ex:200=OK 404="NOT FOUND"
};

#endif /* HTTPRESPONSE_H */
