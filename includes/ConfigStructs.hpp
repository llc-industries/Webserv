#ifndef CONFIG_STRUCTS_HPP
#define CONFIG_STRUCTS_HPP

#include <map>
#include <string>
#include <vector>

struct Location {
  std::string path;                 // .php /index etc...
  std::vector<std::string> methods; // Allowed methods (POST GET DELETE)
  std::pair<int, std::string> ret;  // Return directive
  std::string cgiPath;              // Path to cgi (php pyhton etc)
  std::string uploadPath;           // Path to post requests;
  bool autoindex; // default to off, send ls if request finish with /

  // Priority over server settings
  std::string root;
  std::vector<std::string> index;

  Location() : autoindex(false) { ret.first = -1; }
};

struct ServerConfig {
  std::vector<int> ports;              // listen (default 8080 as non root)
  std::string host;                    // Default to localhost -> 127.0.0.1
  std::string serverName;              // webserv.com
  int maxBodySize;                     // 1000000 == 1 MB (Nginx default)
  std::map<int, std::string> errPages; // Pages to serve for errors

  std::string root;               // Default to /var/www/html
  std::vector<std::string> index; // Default served files
  std::vector<Location> locations;

  ServerConfig() : maxBodySize(-1) {}
};

#endif /* CONFIG_STRUCTS_HPP */
