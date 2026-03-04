1. Implémentation des CGI (CgiHandler.hpp / CgiHandler.cpp)

    Détection et Routage : Déterminer si la route demandée doit passer par un CGI (généralement basé sur l'extension du fichier, comme .php ou .py).
    Environnement : Préparer le char** env requis par execve (variables de base comme REQUEST_METHOD, QUERY_STRING, CONTENT_LENGTH, CONTENT_TYPE, etc.).
    Exécution (fork/pipe) :
        - Créer des pipe() pour rediriger l'entrée et la sortie standard (stdin/stdout).
        - Faire un fork().
        - Dans le processus enfant : utiliser dup2 et lancer execve().

    Asynchronisme : Attention à ne pas bloquer ta boucle epoll. Utiliser waitpid avec WNOHANG ou surveiller le fd du pipe de lecture du CGI dans l'epoll.
    Parsing CGI : Lire la sortie du CGI, séparer ses headers (ex: Content-type: text/html) de son body, et les intégrer dans ta classe HttpResponse.

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

2. Implémentation du Timeout Client (Server.cpp/hpp & Client.cpp/hpp)

    - Tracking du temps : Ajouter une variable time_t _lastActivity dans ta classe Client, initialisée lors de l'appel à acceptConnection().
    - Mise à jour : Mettre à jour _lastActivity avec le temps actuel (time(NULL)) à chaque fois que tu passes dans handleClientRead ou handleClientWrite.

    Nettoyage : Dans Server::run(), ajouter une vérification régulière de ta _clientMap. Si temps_actuel - client._lastActivity > TIMEOUT_LIMIT, appeler closeClient(fd). (Astuce : tu peux mettre un timeout sur epoll_wait au lieu de -1 pour forcer un check régulier même sans trafic).

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

3. Autres éléments bloquants restants

    Méthodes manquantes (Client.cpp) : Les fonctions _handlePost (upload/écriture de fichier) et _handleDelete (suppression avec std::remove()) sont actuellement vides.
    Envoi fragmenté : Comme indiqué dans ton commentaire TODO: ne pas envoyer tout d'un coup, gérer l'envoi partiel dans handleClientWrite en gardant une trace du nombre de bytes déjà envoyés via send(), au cas où un seul appel ne suffise pas.

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------
