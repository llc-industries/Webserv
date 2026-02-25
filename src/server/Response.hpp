#ifndef RESPONSE_H
#define RESPONSE_H

#include <cstdlib>
#include <map>
#include <sstream>
#include <string>

class Response {
private:
  int _status_code;
  std::string _status_message;
  std::map<std::string, std::string> _headers;
  std::string _body;

  std::string getReasonPhrase(int code) const; // ex:200=OK 404="NOT FOUND"
public:
  Response();
  ~Response();

  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &body);
  // MIME
  void autoDetectContentType(const std::string &path);
  std::string toString() const;
};

#endif /* RESPONSE_H */
