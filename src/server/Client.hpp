#ifndef CLIENT_H
#define CLIENT_H

#include "CgiHandler.hpp"
#include "ConfigStructs.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#include <cstdio>
#include <ctime>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>

class Client {
public:
  Client(const ServerConfig *context);
  ~Client();

  void buildResponse();

  // In ClientUtils.cpp
  void swallow(const char *buf, ssize_t bytesRead);
  const char *getResponse() const;
  size_t getResponseLength() const;
  size_t getBytesSent() const;
  void addBytesSent(size_t value);
  bool isRequestComplete() const;
  bool isResponseReady() const;
  std::time_t getLastActivity() const;

  // CGI
  int getCgiFdOut() const { return _cgiFdOut; }
  int getCgiFdIn() const { return _cgiFdIn; }
  pid_t getCgiPid() const { return _cgiPid; }
  void appendCgiOutput(const char *buf, ssize_t bytes) {
    _cgiOutput.append(buf, bytes);
  }
  void parseCgiResponse();
  void cgiTimeoutClean();
  void closeCgiFdOut() {
    if (_cgiFdOut != -1)
      close(_cgiFdOut);
    _cgiFdOut = -1;
  }
  void resetCgi() {
    _cgiFdIn = -1;
    _cgiFdOut = -1;
    _cgiPid = -1;
    _cgiBytesWritten = 0;
  }

private:
  struct Route {
    const Location *loc;
    std::string root;
    std::string full_path;

    Route() : loc(NULL) {}
  };

  const ServerConfig *_context;

  HttpRequest _request;
  bool _isReqComplete;

  HttpResponse _response;
  std::string _rawResponse;
  size_t _bytesSent;
  bool _isRespReady;

  std::time_t _lastActivity;

  // CGI
  int _cgiFdOut;
  int _cgiFdIn;
  size_t _cgiBytesWritten;
  pid_t _cgiPid;
  std::string _cgiOutput;
  void _handleCgi(const Route &route);

  void _handleGet(const Route &route);
  void _handlePost(const Route &route);
  void _handleDelete(const Route &route);

  // GET  utils
  bool _handleDirectory(std::string &target_path, const Route &route);
  // POST utils
  void _parseMultipart(std::string &filename, std::string &content);
  std::string _getUploadDirectory(const Route &route) const;
  void _saveFile(const std::string &save_path, const std::string &content,
                 const std::string &filename);

  // In ClientUtils.cpp
  int _validateRequest() const;
  Route _resolveRoute() const;
  bool _isMethodAllowed(const Route &route) const;
  void _setError(int code);
  bool _isDir(const std::string &path) const;
  std::string _autoIndex(const std::string &path,
                         const std::string &req_path) const;
  bool _multiPart(const std::string &body, const std::string &boundary,
                  std::string &outfile, std::string &outcontent) const;
};

#endif /* CLIENT_H */
