#include "Client.hpp"

void Client::swallow(const char *buf, ssize_t bytesRead) {
  _lastActivity = std::time(NULL);
  _request.swallow(buf, bytesRead, _context->maxBodySize);
  _isReqComplete = _request.isComplete();
}

void Client::_handleRedirection(const Route &route) {
  const std::pair<int, std::string> &ret = route.loc->ret;

  _response.setStatusCode(ret.first);
  _response.setHeader("Location", ret.second);

  _rawResponse = _response.toString();
  _isRespReady = true;
}

/* ---------- GETTERS ---------- */

const char *Client::getResponseStr() const { return _rawResponse.c_str(); }
size_t Client::getResponseStrLength() const { return _rawResponse.length(); }
size_t Client::getBytesSent() const { return _bytesSent; }
const HttpRequest &Client::getRequest() const { return _request; }
const HttpResponse &Client::getResponse() const { return _response; }
void Client::addBytesSent(size_t value) { _bytesSent += value; }
bool Client::isRequestComplete() const { return _isReqComplete; }
bool Client::isResponseReady() const { return _isRespReady; }
std::time_t Client::getLastActivity() const { return _lastActivity; }

/* ---------- RESPONSE HELPERS ---------- */

// Check si la request est bien formée
int Client::_validateRequest() const {
  // Error 413 -> Entity Too Large
  if (_request.getErrorCode() != 0)
    return _request.getErrorCode();

  // Error 400 -> Bad request
  if (_request.getPath().empty() || _request.getMethod().empty() ||
      _request.getHeader("Host").empty())
    return 400;

  return 0;
}

Client::Route Client::_resolveRoute() const {
  Route ret;
  std::string path = _request.getPath();

  size_t query_pos = path.find('?');
  if (query_pos != std::string::npos)
    path = path.substr(0, query_pos);

  size_t maxLen = 0;
  for (size_t i = 0; i < _context->locations.size(); i++) {
    const std::string &locPath = _context->locations[i].path;

    if (locPath.length() > 1 && locPath[0] == '*') {
      std::string ext = locPath.substr(1);
      if (path.length() >= ext.length() &&
          path.compare(path.length() - ext.length(), ext.length(), ext) == 0) {
        ret.loc = &(_context->locations[i]);
        break;
      }
    } else if (path.find(locPath) == 0 && locPath.length() > maxLen) {
      ret.loc = &(_context->locations[i]);
      maxLen = locPath.length();
    }
  }

  if (ret.loc != NULL)
    ret.root = ret.loc->root;
  else
    ret.root = _context->root;

  ret.full_path = ret.root + path;

  if (path == "/") {
    if (ret.loc != NULL && ret.loc->index.empty() == false)
      ret.full_path = ret.root + "/" + ret.loc->index[0];
    else if (ret.loc == NULL && _context->index.empty() == false)
      ret.full_path = ret.root + "/" + _context->index[0];
  }

  return ret;
}

// Check POST GET etc
bool Client::_isMethodAllowed(const Route &route) const {
  if (route.loc == NULL || route.loc->methods.empty())
    return true;

  std::string method = _request.getMethod();
  for (size_t i = 0; i < route.loc->methods.size(); i++) {
    if (route.loc->methods[i] == method)
      return true;
  }
  return false;
}

// Set page to return
void Client::_setError(int code) {
  _response.setStatusCode(code);
  std::string errorFile;

  std::map<int, std::string>::const_iterator it = _context->errPages.find(code);
  if (it != _context->errPages.end())
    errorFile = _context->root + it->second; // Fichier de config
  else {
    std::ostringstream ss;
    ss << "./www/error_pages/" << code << ".html";
    errorFile = ss.str(); // Fichier par défaut
  }

  std::ifstream file(errorFile.c_str());
  if (file.is_open() == true) { // On sert le fichier
    std::ostringstream os;
    os << file.rdbuf();
    _response.setBody(os.str());
    _response.setHeader("Content-Type", "text/html");
  } else { // On génère en dur
    std::ostringstream os;
    os << "<html><body><h1>" << code << " " << _response.getReasonPhrase(code)
       << "</h1></body></html>";
    _response.setBody(os.str());
    _response.setHeader("Content-Type", "text/html");
  }

  _rawResponse = _response.toString();
  _isRespReady = true;
}

bool Client::_isDir(const std::string &path) const {
  struct stat s;
  if (stat(path.c_str(), &s) == 0) {
    return S_ISDIR(s.st_mode);
  }
  return false;
}

std::string Client::_autoIndex(const std::string &path,
                               const std::string &req_path) const {
  DIR *dir;
  struct dirent *ent;
  std::ostringstream html;
  html << "<html><head><title>Index of " << req_path << "</title></head>";
  html << "<body style='font-family: Arial, sans-serif;'>";
  html << "<h1>Index of " << req_path << "</h1><hr><ul>";
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string name = ent->d_name;
      if (name == ".")
        continue;
      std::string slash = (req_path[req_path.size() - 1] == '/') ? "" : "/";
      html << "<li><a href=\"" << req_path << slash << name << "\">" << name
           << "</a></li>";
    }
    closedir(dir);
  }
  html << "</ul><hr></body></html>";
  return html.str();
}

bool Client::_multiPart(const std::string &body, const std::string &boundary,
                        std::string &out_filename,
                        std::string &out_content) const {
  std::string delimiter = "--" + boundary;
  size_t pos = body.find(delimiter);

  if (pos == std::string::npos)
    return false;

  // le nom du fichier
  size_t filename_pos = body.find("filename=\"", pos);
  if (filename_pos == std::string::npos)
    return false;

  filename_pos += 10; // avance après 'filename="'
  size_t filename_end = body.find("\"", filename_pos);
  out_filename = body.substr(filename_pos, filename_end - filename_pos);
  size_t content_start = body.find("\r\n\r\n", filename_end);
  if (content_start == std::string::npos)
    return false;
  content_start += 4;

  // fin du fichier
  size_t content_end = body.find(delimiter, content_start);
  if (content_end == std::string::npos)
    return false;

  content_end -= 2; // enlève le délimiteur de fin
  out_content = body.substr(content_start, content_end - content_start);
  return true;
}

void Client::updateActivity() { _lastActivity = std::time(NULL); }

int Client::getCgiFdOut() const { return _cgiFdOut; }
int Client::getCgiFdIn() const { return _cgiFdIn; }
const std::string &Client::getRequestBody() { return _request.getBody(); }
size_t Client::getCgiBytesWritten() const { return _cgiBytesWritten; }
void Client::addCgiBytesWritten(size_t bytes) { _cgiBytesWritten += bytes; }
pid_t Client::getCgiPid() const { return _cgiPid; }
void Client::appendCgiOutput(const char *buf, ssize_t bytes) {
  _cgiOutput.append(buf, bytes);
}

void Client::closeCgiFdOut() {
  if (_cgiFdOut != -1)
    close(_cgiFdOut);
  _cgiFdOut = -1;
}
void Client::closeCgiFdIn() {
  if (_cgiFdIn != -1)
    close(_cgiFdIn);
  _cgiFdIn = -1;
}
void Client::resetCgi() {
  _cgiFdIn = -1;
  _cgiFdOut = -1;
  _cgiPid = -1;
  _cgiBytesWritten = 0;
}
