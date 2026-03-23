#include "Client.hpp"

Client::Client(const ServerConfig *context)
    : _context(context), _isReqComplete(false), _bytesSent(0),
      _isRespReady(false), _lastActivity(std::time(NULL)), _cgiFdOut(-1),
      _cgiFdIn(-1), _cgiBytesWritten(0), _cgiPid(-1) {}

Client::~Client() {}

void Client::buildResponse() {
  int errCode = _validateRequest();
  if (errCode != 0) {
    _setError(errCode);
    return;
  }

  Route route = _resolveRoute();

  if (route.loc != NULL && route.loc->ret.first != -1) {
    _handleRedirection(route);
    return;
  }

  if (_isMethodAllowed(route) == false) {
    _setError(405);
    return;
  }

  if (route.loc != NULL && !route.loc->cgiPath.empty()) {
    _handleCgi(route);
    return;
  }
  if (_request.getMethod() == "GET")
    _handleGet(route);
  else if (_request.getMethod() == "DELETE")
    _handleDelete(route);
  else if (_request.getMethod() == "POST")
    _handlePost(route);
  else
    _setError(501);
}

/* ---------- HANDLE GET POST DELETE ---------- */

void Client::_handleGet(const Route &route) {
  std::string target_path = route.full_path;

  if (_isDir(target_path)) {
    if (_handleDirectory(target_path, route) == true)
      return;
  }

  std::ifstream file(target_path.c_str(), std::ios::binary);

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

bool Client::_handleDirectory(std::string &target_path, const Route &route) {
  std::string index_path =
      target_path + (target_path[target_path.size() - 1] == '/' ? "" : "/") +
      "index.html";
  std::ifstream index_file(index_path.c_str());

  if (index_file.is_open()) {
    target_path = index_path;
    return false;
  }
  if (route.loc != NULL && route.loc->autoindex == true) {
    _response.setStatusCode(200);
    _response.setHeader("Content-Type", "text/html");
    _response.setBody(_autoIndex(target_path, _request.getPath()));

    std::string full_res = _response.toString();
    _rawResponse = _response.toString();
    _isRespReady = true;
    return true;
  }

  _setError(403);
  return true;
}

void Client::_handlePost(const Route &route) {
  std::string upload_dir = _getUploadDirectory(route);

  if (upload_dir.empty()) {
    _setError(403);
    return;
  }

  std::string body = _request.getBody();
  std::stringstream ss;
  ss << "upload_" << std::time(NULL) << "_" << std::rand() % 1000 << ".bin";
  std::string filename = ss.str();

  _parseMultipart(filename, body);
  std::string path = upload_dir + filename;
  _saveFile(path, body, filename);
}

void Client::_parseMultipart(std::string &filename, std::string &content) {
  std::string type = _request.getHeader("Content-Type");

  if (type.find("multipart/form-data") != std::string::npos) {
    size_t boundary_pos = type.find("boundary=");
    if (boundary_pos != std::string::npos) {
      std::string boundary = type.substr(boundary_pos + 9);
      std::string extracted_filename;
      std::string extracted_content;

      if (_multiPart(content, boundary, extracted_filename,
                     extracted_content)) {
        if (!extracted_filename.empty()) {
          filename = extracted_filename;
          content = extracted_content;
        }
      }
    }
  }
}

std::string Client::_getUploadDirectory(const Route &route) const {
  if (route.loc != NULL && route.loc->uploadPath.empty() == false) {
    std::string upload_dir = route.root;

    if (upload_dir.empty() == false &&
        upload_dir[upload_dir.length() - 1] != '/')
      upload_dir += "/";

    upload_dir += route.loc->uploadPath;
    if (upload_dir[upload_dir.length() - 1] != '/')
      upload_dir += '/';
    return upload_dir;
  }
  return "";
}

void Client::_saveFile(const std::string &save_path, const std::string &content,
                       const std::string &filename) {
  std::ofstream outfile(save_path.c_str(), std::ios::binary);

  if (!outfile.is_open()) {
    _setError(500);
    return;
  }

  outfile.write(content.c_str(), content.size());
  outfile.close();

  _response.setStatusCode(201);
  _response.setHeader("Content-Type", "text/html");
  _response.setBody("<html><body><h1>Upload reussi ! Fichier : " + filename +
                    " sauvegarde.</h1></body></html>");

  _rawResponse = _response.toString();
  _isRespReady = true;
}

void Client::_handleDelete(const Route &route) {
  if (std::remove(route.full_path.c_str()) == 0) {
    _response.setStatusCode(200);
    _response.setHeader("Content-Type", "text/html");
    _response.setBody("<html><body><h1>200 OK : Fichier supprime avec succes "
                      "!</h1></body></html>");
  } else {
    _setError(404);
    return;
  }

  _rawResponse = _response.toString();
  _isRespReady = true;
}

void Client::_handleCgi(const Route &route) {
  CgiHandler cgi(_request, route.full_path, route.loc->cgiPath);

  if (cgi.executeCgi(_cgiPid, _cgiFdOut, _cgiFdIn) == -1)
    _setError(500);
}

void Client::parseCgiResponse() {
  if (_cgiOutput.empty()) {
    _setError(500);
    return;
  }
  size_t header_end = _cgiOutput.find("\r\n\r\n");
  size_t sep_len = 4;

  if (header_end == std::string::npos) {
    header_end = _cgiOutput.find("\n\n");
    sep_len = 2;
  }

  std::string header_str;
  std::string body;

  if (header_end != std::string::npos) {
    header_str = _cgiOutput.substr(0, header_end);
    body = _cgiOutput.substr(header_end + sep_len);
  } else {
    body = _cgiOutput;
  }

  int status_code = 200;
  std::string content_type = "text/html";

  std::istringstream iss(header_str);
  std::string line;
  while (std::getline(iss, line)) {
    if (line.empty() == false && line[line.length() - 1] == '\r')
      line.erase(line.length() - 1);

    size_t colon = line.find(":");
    if (colon != std::string::npos) {
      std::string key = line.substr(0, colon);
      std::string value = line.substr(colon + 1);

      size_t start = value.find_first_not_of(" \t");
      if (start != std::string::npos)
        value = value.substr(start);

      if (key == "Status")
        status_code = std::atoi(value.c_str());
      else if (key == "Content-Type" || key == "Content-type")
        content_type = value;
      else
        _response.setHeader(key, value);
    }
  }
  _response.setStatusCode(status_code);
  _response.setBody(body);
  _response.setHeader("Content-Type", content_type);
  _rawResponse = _response.toString();
  _isRespReady = true;
}

void Client::cgiTimeoutClean() {
  _setError(502);
  resetCgi();
  _lastActivity = std::time(NULL);
}

void Client::cgiCrash() { _setError(500); }
