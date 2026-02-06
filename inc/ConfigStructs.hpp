#ifndef CONFIG_STRUCTS_HPP
#define CONFIG_STRUCTS_HPP

#include <map>
#include <string>
#include <vector>

struct Location {
  std::string path;                 // .php /index etc...
  std::string root;                 //
  std::vector<std::string> methods; // Allowed methods (POST GET DELETE)
  std::vector<std::string> index;
  std::string cgiPass;             // Path to cgi (php pyhton etc)
  std::string uploadStore;         // Path to post requests;
  std::pair<int, std::string> ret; // Return directive
  bool autoindex; // default to off, send ls if request finish with /

  Location() : autoindex(false) { ret.first = 0; }
};

struct ServerConfig {
  int port;                       // HTTP default is 8080 as non root
  int maxBodySize;                // 1000000 == 1 MB (Nginx default)
  std::string host;               // Default to localhost -> 127.0.0.1
  std::string serverName;         // Defaulting to webserv.com
  std::string root;               // Default to /var/www/html
  std::vector<std::string> index; // Default served files
  std::map<int, std::string> errPages;
  std::vector<Location> locations;

  ServerConfig() : port(-1), maxBodySize(-1) {}
};

#endif /* CONFIG_STRUCTS_HPP */
