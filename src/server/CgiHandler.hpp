#ifndef CGIHANDLER_H
#define CGIHANDLER_H

#include "HttpRequest.hpp"
#include <string>
#include <map>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>

class CgiHandler{
    public:
        CgiHandler(const HttpRequest& request, const std::string& scriptPath, const std::string& cgiBinPath);
        ~CgiHandler();

        std::string executeCgi();
    private:
        const HttpRequest& _request;
        std::string _scriptPath;
        std::string _cgiBinPath;

        void _initEnv();//initialise les variables d'env 
};


#endif /* CGIHANDLER_H */
