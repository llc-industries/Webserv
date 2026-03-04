#include "Client.hpp"

Client::Client(const ServerConfig *context)
    : _context(context), _isReqComplete(false), _isRespReady(false) {}

Client::~Client() {}

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
    std::string index_path =
        target_path + (target_path[target_path.size() - 1] == '/' ? "" : "/") +
        "index.html";
    std::ifstream index_file(index_path.c_str());

    if (index_file.is_open()) {
      target_path = index_path;
    } else {
      if (route.loc != NULL && route.loc->autoindex == true) {
        _response.setStatusCode(200);
        _response.setHeader("Content-Type", "text/html");
        _response.setBody(_autoIndex(target_path, _request.getPath()));

        std::string full_res = _response.toString();
        _rawResponse = _response.toString();
        _isRespReady = true;
        return;
      } else {
        _setError(403);
        return;
      }
    }
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

void Client::_handlePost(const Route &route) {
  std::string body_content = _request.getBody();
  std::string content_type = _request.getHeader("Content-Type");

  std::string save_filename = "uploaded_file.txt";
  std::string final_content = body_content;

  if (content_type.find("multipart/form-data") != std::string::npos) {
    size_t boundary_pos = content_type.find("boundary=");
    if (boundary_pos != std::string::npos) {
      std::string boundary = content_type.substr(boundary_pos + 9);
      std::string extracted_filename;
      std::string extracted_content;

      if (_multiPart(body_content, boundary, extracted_filename,
                     extracted_content)) {
        if (!extracted_filename.empty()) {
          save_filename = extracted_filename;
          final_content = extracted_content;
        }
      }
    }
  }
  std::string save_path = "./www/upload/" + save_filename;
  std::ofstream outfile(save_path.c_str(), std::ios::binary);

  if (outfile.is_open()) {
    outfile.write(final_content.c_str(), final_content.size());
    outfile.close();

    _response.setStatusCode(201);
    _response.setHeader("Content-Type", "text/html");
    _response.setBody("<html><body><h1>Upload reussi ! Fichier : " +
                      save_filename + " sauvegarde.</h1></body></html>");
  } else {
    _setError(500);
    return;
  }

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
