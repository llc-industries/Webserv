#include "Client.hpp"

Client::Client(const ServerConfig *context)
    : _context(context), _isReqComplete(false), _isRespReady(false) {}

Client::~Client() {}

void Client::swallow(const char *buf, ssize_t bytesRead) {
  _request.swallow(buf, bytesRead);
  _isReqComplete = _request.isComplete();
}

/* ---------- GETTERS ---------- */

const char *Client::getResponse() const { return _rawResponse.c_str(); }
size_t Client::getResponseLength() const { return _rawResponse.length(); }
bool Client::isRequestComplete() const { return _isReqComplete; }
bool Client::isResponseReady() const { return _isRespReady; }

/* ---------- BUILD RESPONSE ---------- */

// C'est la main fonction qui prépare la réponse en tandem avec
// _request et _response
void Client::buildResponse() {
  int errCode = _validateRequest();
  if (errCode != 0) {
    _setError(errCode);
    return;
  }

  Route route = _resolveRoute();

  if (_isMethodAllowed(route) == false) {
    _setError(405);
    return;
  }

  if (_request.getMethod() == "GET")
    _handleGet(route);
  // Commenté pour compiler
  /*   else if (_request.getMethod() == "DELETE")
      _handleDelete(route);
    else if (_request.getMethod() == "POST")
      _handlePost(route); */
  else
    _setError(501);
}

/* ---------- RESPONSE HELPERS ---------- */

// CHecl si la request est bien formée
int Client::_validateRequest() const {
  // Error 400 -> Bad request
  if (_request.getPath().empty() || _request.getMethod().empty() ||
      _request.getHeader("Host").empty())
    return 400;

  // Error 413 -> Entity Too Large
  if (_request.getMethod() == "POST" && _context->maxBodySize > 0)
    if (_request.getBody().size() > static_cast<size_t>(_context->maxBodySize))
      return 413;

  return 0;
}

// Prépare la structure route
// route contiens:
// Le bloc location si besoin
// Les path root et full path
Client::Route Client::_resolveRoute() const {

  Route ret;
  size_t maxLen = 0;
  std::string path = _request.getPath();

  // Find appropriate location
  for (size_t i = 0; i < _context->locations.size(); i++) {
    const std::string &locPath = _context->locations[i].path;
    if (path.find(locPath) == 0 && locPath.length() > maxLen) {
      ret.loc = &(_context->locations[i]);
      maxLen = locPath.length();
    }
  }

  // Set root + full path
  if (ret.loc != NULL && ret.loc->root.empty() == false)
    ret.root = ret.loc->root;
  else
    ret.root = _context->root;

  ret.full_path = ret.root + path;

  if (path == "/") {
    if (ret.loc && ret.loc->index.empty() == false)
      ret.full_path = ret.root + "/" + ret.loc->index[0];
    else if (_context->index.empty() == false)
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

/* ---------- HANDLE GET POST DELETE ---------- */

void Client::_handleGet(const Route &route) {
  std::ifstream file(route.full_path.c_str(), std::ios::binary);

  if (!file.is_open()) {
    _setError(404);
    return;
  }

  std::ostringstream file_content;
  file_content << file.rdbuf();

  _response.setStatusCode(200);
  _response.autoDetectContentType(route.full_path);
  _response.setBody(file_content.str());

  _rawResponse = _response.toString();
  _isRespReady = true;
}

void Client::_handlePost(const Route &route) {}
void Client::_handleDelete(const Route &route) {}

// Set page to return
void Client::_setError(int code) {}
