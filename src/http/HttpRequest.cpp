#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
    : _headers_parsed(false), _is_complete(false), _is_chunked(false),
      _body_pos(0), _content_length(0), _error_code(0) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::swallow(const char *buffer, size_t bytes, int maxBodySize) {
  if (_error_code != 0)
    return;

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
      _body_pos = end_of_headers + 4;

      if (maxBodySize > 0 &&
          _content_length > static_cast<size_t>(maxBodySize)) {
        _error_code = 413;
        _is_complete = true;
        return;
      }
    }
  }

  if (_headers_parsed) {
    if (_is_chunked == true) {
      parseChunkedBody(maxBodySize);
    } else if (_method == "POST") {
      size_t body_start = _raw_data.find("\r\n\r\n") + 4;
      size_t current_body_size = _raw_data.size() - body_start;
      if (maxBodySize > 0 &&
          _content_length > static_cast<size_t>(maxBodySize)) {
        _error_code = 413;
        _is_complete = true;
        return;
      }
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
    if (key == "Cookie") {
      std::stringstream ss(value);
      std::string pair;
      while (std::getline(ss, pair, ':')) {
        size_t sep = pair.find('=');
        if (sep != std::string::npos) {
          std::string c_key = pair.substr(pair.find_first_not_of(" "), sep);
          std::string c_value = pair.substr(sep + 1);
          _cookies[c_key] = c_value;
        }
      }
    } else if (key == "Content-Length") {
      _content_length = std::atoi(value.c_str());
    } else if (key == "Transfer-Encoding" &&
               value.find("chunked") != std::string::npos) {
      _is_chunked = true;
    }
  }
}

void HttpRequest::parseChunkedBody(int maxBodySize) {
  while (42) {
    size_t crlf_pos = _raw_data.find("\r\n", _body_pos);
    if (crlf_pos == std::string::npos) {
      break;
    }

    std::string hex_str = _raw_data.substr(_body_pos, crlf_pos - _body_pos);
    size_t chunk_size = 0;
    std::stringstream ss;
    ss << std::hex << hex_str;
    ss >> chunk_size;

    if (chunk_size == 0) {
      if (_raw_data.size() >= crlf_pos + 4)
        _is_complete = true;
      break;
    }

    if (maxBodySize > 0 &&
        _body.size() + chunk_size > static_cast<size_t>(maxBodySize)) {
      _error_code = 413;
      _is_complete = true;
      return;
    }

    size_t next_chunk_start = crlf_pos + chunk_size + 4;
    if (_raw_data.size() >= next_chunk_start) {
      _body.append(_raw_data, crlf_pos + 2, chunk_size);
      _body_pos = next_chunk_start;
    } else {
      break;
    }
  }
}

bool HttpRequest::isComplete() const { return _is_complete; }

std::string HttpRequest::getMethod() const { return _method; }

std::string HttpRequest::getPath() const { return _path; }
int HttpRequest::getErrorCode() const { return _error_code; }
const std::string &HttpRequest::getBody() const { return _body; }

std::string HttpRequest::getHeader(const std::string &key) const {
  std::map<std::string, std::string>::const_iterator it = _headers.find(key);
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}
