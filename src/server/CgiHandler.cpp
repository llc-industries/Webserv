#include "CgiHandler.hpp"

CgiHandler::CgiHandler(const HttpRequest &request,
                       const std::string &scriptPath,
                       const std::string &cgiBinPath)
    : _request(request), _scriptPath(scriptPath), _cgiBinPath(cgiBinPath) {
  _initEnv();
}

CgiHandler::~CgiHandler() {}

void CgiHandler::_initEnv() {
  _envMap["REQUEST_METHOD"] = _request.getMethod();
  _envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
  _envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
  _envMap["SCRIPT_FILENAME"] = _scriptPath;
  _envMap["REDIRECT_STATUS"] = "200";
  if (_request.getMethod() == "POST") {
    std::ostringstream ss;
    ss << _request.getBody().length();
    _envMap["CONTENT_LENGTH"] = ss.str();
    _envMap["CONTENT_TYPE"] = _request.getHeader("Content-Type");
  }

  size_t queryPos = _request.getPath().find('?');
  if (queryPos != std::string::npos) {
    _envMap["QUERY_STRING"] = _request.getPath().substr(queryPos + 1);
  } else {
    _envMap["QUERY_STRING"] = "";
  }
}

// la norme CGI nous obliges a creer dans le env ces variable:
//      REQUEST_METHOD : "GET" ou "POST"
//      QUERY_STRING : Ce qu'il y a après le ? dans l'URL (ex: id=42&name=anas)
//      CONTENT_LENGTH : La taille du body (très important pour le POST)
//      CONTENT_TYPE : Le type du body (ex: application/x-www-form-urlencoded)
//      SCRIPT_FILENAME : Le chemin absolu vers le fichier .php sur ton disque
//      dur. REDIRECT_STATUS=200 : Astuce vitale -> php-cgi refuse souvent de
//      s'exécuter si cette variable n'est pas présente pour des raisons de
//      sécurité.

char **CgiHandler::_getEnvArray() const {
  char **envp = new char *[_envMap.size() + 1];
  int i = 0;
  for (std::map<std::string, std::string>::const_iterator it = _envMap.begin();
       it != _envMap.end(); ++it) {
    std::string envStr = it->first + "=" + it->second;
    envp[i] = new char[envStr.length() + 1];
    std::strcpy(envp[i], envStr.c_str());
    i++;
  }
  envp[i] = NULL;
  return envp;
} // convertir notre map en char ** pour l'execve

int CgiHandler::executeCgi(pid_t &out_pid, int &out_fd, int &in_fd) {
  int pipe_in[2];
  int pipe_out[2];

  if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
    return -1;

  pid_t pid = fork();
  if (pid < 0)
    return -1;

  if (pid == 0) {      // proc enfant
    close(pipe_in[1]); // on empeche le script de lire et d'ecouter les pipe
    close(pipe_out[0]);
    dup2(pipe_in[0], STDIN_FILENO); // on branche le cin et le cout au pipe
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_in[0]);
    close(pipe_out[1]);

    char **envp = _getEnvArray();
    char *args[3];
    args[0] = const_cast<char *>(_cgiBinPath.c_str());
    args[1] = const_cast<char *>(_scriptPath.c_str());
    args[2] = NULL;

    execve(args[0], args, envp);
    exit(EXIT_FAILURE); // si execve echoue le runtime continue ici
  } else {
    close(pipe_in[0]);
    close(pipe_out[1]);

    fcntl(pipe_out[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe_in[1], F_SETFL, O_NONBLOCK);

    out_pid = pid;
    out_fd = pipe_out[0];
    in_fd = pipe_in[1];
    return 0;
  }
}

// pipe()
// pipe_in->le serveur recoit le body (le script) de la requete en stdin
// pipe_out->le script va etre executer et renvoie sa reponse sur stdout

// fork()
// execute le script dans le processus fils
// le serveur tournera normalement sur le processus pere

// dup2()
// utiliser dans le proc fils pour communique au serveur
// branche le stdin et stdout au tuyau pipe_in pipe_out

// execve
// execute le script dans /usr/bin/(php-cgi ou python3) en lui donnant le path
// du script
