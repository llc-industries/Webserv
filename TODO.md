1. Le Timeout CGI (Obligatoire pour la sécurité)

Actuellement, si tu lances infinite.php, ton serveur ne crashera pas (grâce à epoll), mais le processus PHP va tourner éternellement en arrière-plan et consommer la RAM/CPU de l'ordinateur de l'école.

    Ce qu'il faut faire : Dans ta fonction _handleTimeouts(), en plus de vérifier l'inactivité des clients, tu dois vérifier depuis combien de temps chaque _cgiPid tourne. S'il dépasse 10 secondes, tu fais kill(client.getCgiPid(), SIGKILL) et tu envoies une page 504 Gateway Timeout au client.

2. Améliorer parseCgiResponse (Protection contre les crashs CGI)

Si le CGI crashe, ta fonction actuelle parseCgiResponse cherche \r\n\r\n et envoie un code 200 même si la chaîne est vide.

    Ce qu'il faut faire : Ajouter une sécurité : if (_cgiOutput.empty() || _cgiOutput.find("Status: 500") != std::string::npos) { _setError(502); return; } pour renvoyer une belle "Bad Gateway" si le script a planté.
