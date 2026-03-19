#ifndef CLIENT_H
#define CLIENT_H

#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "CgiHandler.hpp"
#include "ConfigStructs.hpp"

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
  const char *getResponseStr() const;
  size_t getResponseStrLength() const;
  size_t getBytesSent() const;
  const HttpRequest &getRequest() const;
  const HttpResponse &getResponse() const;
  void addBytesSent(size_t value);
  bool isRequestComplete() const;
  bool isResponseReady() const;
  std::time_t getLastActivity() const;
  void updateActivity();

  // CGI
  int getCgiFdOut() const;
  int getCgiFdIn() const;
  const std::string &getRequestBody();
  size_t getCgiBytesWritten() const;
  void addCgiBytesWritten(size_t bytes);
  pid_t getCgiPid() const;
  void appendCgiOutput(const char *buf, ssize_t bytes);
  void parseCgiResponse();
  void cgiTimeoutClean();
  void closeCgiFdOut();
  void closeCgiFdIn();
  void resetCgi();

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
  void _handleRedirection(const Route &route);
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
