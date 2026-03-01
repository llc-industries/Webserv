#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
    : _headers_parsed(false), _is_complete(false), _content_length(0) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::swallow(const char *buffer, size_t bytes) {
  _raw_data.append(buffer, bytes);
  if (!_headers_parsed) {
    size_t end_of_headers = _raw_data.find("\r\n\r\n");
    if (end_of_headers != std::string::npos) {
      std::string header_part = _raw_data.substr(0, end_of_headers);
      std::istringstream stream(header_part);
      std::string line;

      if (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
          line.erase(line.size() - 1);
        parseFirstLine(line);
      }
      while (std::getline(stream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
          line.erase(line.size() - 1);
        if (!line.empty())
          parseHeaderLine(line);
      }
      _headers_parsed = true;
    }
  }
  if (_headers_parsed) {
    if (_method == "POST") {
      size_t body_start = _raw_data.find("\r\n\r\n") + 4;
      size_t current_body_size = _raw_data.size() - body_start;
      if (current_body_size >= _content_length) {
        _is_complete = true;
        _body = _raw_data.substr(body_start, _content_length);
      }
    } else {
      _is_complete = true;
    }
  }
}

void HttpRequest::parseFirstLine(const std::string &line) {
  std::istringstream iss(line);
  iss >> _method;
  iss >> _path;
  iss >> _version;
}

void HttpRequest::parseHeaderLine(const std::string &line) {
  size_t pos = line.find(':');
  if (pos != std::string::npos) {
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    size_t value_start = value.find_first_not_of(" \t");
    if (value_start != std::string::npos) {
      value = value.substr(value_start);
    }
    _headers[key] = value;
    if (key == "Content-Length") {
      _content_length = std::atoi(value.c_str());
    }
  }
}

bool HttpRequest::isComplete() const { return _is_complete; }

std::string HttpRequest::getMethod() const { return _method; }

std::string HttpRequest::getPath() const { return _path; }

std::string HttpRequest::getBody() const { return _body; }

std::string HttpRequest::getHeader(const std::string &key) const {
  std::map<std::string, std::string>::const_iterator it = _headers.find(key);
  if (it != _headers.end()) {
    return it->second;
  }
  return ""; // si ya pas de header
}
