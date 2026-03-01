#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status_code(200), _status_message("OK") {
  _headers["Server"] = "Webserv/1.0";
  _headers["Connection"] = "close";
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(int code) {
  _status_code = code;
  _status_message = getReasonPhrase(code);
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
  _headers[key] = value;
}

void HttpResponse::setBody(const std::string &body) {
  _body = body;
  std::ostringstream length_str;
  length_str << _body.size();
  setHeader("Content-Length", length_str.str());
}

std::string HttpResponse::getReasonPhrase(int code) const {
  switch (code) {
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 301:
    return "Moved Permanently";
  case 400:
    return "Bad Request";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 413:
    return "Payload Too Large";
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  default:
    return "Unknown Status";
  }
}

void HttpResponse::autoDetectContentType(const std::string &path) {

  size_t dotPos = path.find_last_of(".");
  if (dotPos == std::string::npos) {
    setHeader("Content-Type", "text/plain");
    return;
  }

  std::string ext = path.substr(dotPos);

  if (ext == ".html" || ext == ".htm")
    setHeader("Content-Type", "text/html");
  else if (ext == ".css")
    setHeader("Content-Type", "text/css");
  else if (ext == ".js")
    setHeader("Content-Type", "application/javascript");
  else if (ext == ".png")
    setHeader("Content-Type", "image/png");
  else if (ext == ".jpg" || ext == ".jpeg")
    setHeader("Content-Type", "image/jpeg");
  else if (ext == ".gif")
    setHeader("Content-Type", "image/gif");
  else if (ext == ".ico")
    setHeader("Content-Type", "image/x-icon");
  else if (ext == ".pdf")
    setHeader("Content-Type", "application/pdf");
  else if (ext == ".txt")
    setHeader("Content-Type", "text/plain");
  else
    setHeader("Content-Type", "application/octet-stream");
}

std::string HttpResponse::toString() const {
  std::ostringstream HttpResponse;
  HttpResponse << "HTTP/1.1 " << _status_code << " " << _status_message
               << "\r\n";
  for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
       it != _headers.end(); ++it) {
    HttpResponse << it->first << ": " << it->second << "\r\n";
  }
  HttpResponse << "\r\n";
  HttpResponse << _body;
  return HttpResponse.str();
}
