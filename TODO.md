1. Timeout et Déconnexions

    Tu dois gérer proprement la déconnexion des clients.

    Une requête vers ton serveur ne doit jamais rester bloquée indéfiniment. Il faut implémenter un check de dernière activité (timeout).

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

2. Implémentation du CGI

    Déclencher l'exécution basée sur l'extension du fichier.

    Transmettre la requête complète et les arguments au CGI via les variables d'environnement.

    Lancer le CGI depuis le bon répertoire pour que les chemins relatifs fonctionnent.

    Lire la sortie du CGI : s'il ne renvoie pas de Content-Length, utiliser le EOF pour détecter la fin des données.

    Configurer au moins un CGI opérationnel pour l'évaluation (ex: Python ou PHP).
