#include "CgiHandler.hpp"

CgiHandler::CgiHandler(const HttpRequest& request, const std::string& scriptPath, const std::string& cgiBinPath)
    : _request(request), _scriptPath(scriptPath), _cgiBinPath(cgiBinPath){
        _initEnv();
}

CgiHandler::~CgiHandler() {}

void CgiHandler::_initEnv() {}

//la norme CGI nous obliges a creer dans le env ces variable:
//     REQUEST_METHOD : "GET" ou "POST"
//     QUERY_STRING : Ce qu'il y a après le ? dans l'URL (ex: id=42&name=anas)
//     CONTENT_LENGTH : La taille du body (très important pour le POST)
//     CONTENT_TYPE : Le type du body (ex: application/x-www-form-urlencoded)
//     SCRIPT_FILENAME : Le chemin absolu vers le fichier .php sur ton disque dur.
//     REDIRECT_STATUS=200 : Astuce vitale -> php-cgi refuse souvent de s'exécuter si cette variable n'est pas présente pour des raisons de sécurité.


std::string CgiHandler::executeCgi() {}

//pipe() 
//pipe_in->le serveur recoit le body (le script) de la requete en stdin
//pipe_out->le script va etre executer et renvoie sa reponse sur stdout

//fork()
//execute le script dans le processus fils 
//le serveur tournera normalement sur le processus pere

//dup2()
//utiliser dans le proc fils pour communique au serveur
//branche le stdin et stdout au tuyau pipe_in pipe_out

//execve
//execute le script dans /usr/bin/(php-cgi ou python3) en lui donnant le path du script
