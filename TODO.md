1. Envoi fragmenté (send non-bloquant)

    Ton serveur doit rester non-bloquant en toutes circonstances.

    Tu ne dois jamais faire de write ou send sans passer par epoll. Il faut donc gérer le cas où send() n'envoie qu'une partie de _rawResponse et reprendre l'envoi au prochain tour de boucle.

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

2. Timeout et Déconnexions

    Tu dois gérer proprement la déconnexion des clients.

    Une requête vers ton serveur ne doit jamais rester bloquée indéfiniment. Il faut implémenter un check de dernière activité (timeout).

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

3. Implémentation du CGI

    Déclencher l'exécution basée sur l'extension du fichier.

    Transmettre la requête complète et les arguments au CGI via les variables d'environnement.

    Lancer le CGI depuis le bon répertoire pour que les chemins relatifs fonctionnent.

    Lire la sortie du CGI : s'il ne renvoie pas de Content-Length, utiliser le EOF pour détecter la fin des données.

    Configurer au moins un CGI opérationnel pour l'évaluation (ex: Python ou PHP).

-------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------

4. Signal Handler (Arrêt propre)

    Intercepter SIGINT (Ctrl+C) via la fonction signal().

    Utiliser une variable globale extern volatile sig_atomic_t pour changer l'état d'exécution.

    Remplacer ta boucle infinie while (42) dans Server::run() par une vérification de cette variable. La sortie de la boucle détruira l'instance de Server et appellera son destructeur pour fermer proprement tous les FDs et l'instance epoll.
