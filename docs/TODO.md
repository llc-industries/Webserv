TODO : Implémenter les Cookies et la Gestion de Session (Bonus)

    Parsing des Cookies (Lecture dans HttpRequest) :
        Détecter et intercepter l'en-tête Cookie: lors de l'analyse de la requête.
        Découper la chaîne reçue (les cookies sont séparés par des ; ) et stocker les paires clé=valeur dans un conteneur facile d'accès (ex: std::map<std::string, std::string> _cookies).

    Génération d'ID de Session :
        Implémenter un générateur d'identifiants uniques pour les nouvelles sessions (une chaîne alphanumérique aléatoire générée via /dev/urandom ou std::rand() seedé avec std::time).

    Stockage côté Serveur (Session Management) :
        Ajouter une structure de données dans la classe Server (ex: std::map<std::string, std::string> _sessions) pour lier l'ID de session généré à des données spécifiques à l'utilisateur (nom, statut de connexion, ou simple compteur).
        Prévoir un mécanisme de nettoyage (un timestamp de dernière activité) pour supprimer les sessions inactives et éviter de saturer la RAM.

    Création du Cookie (Écriture dans HttpResponse) :
        Ajouter une méthode pour formater et injecter l'en-tête sortant. Exemple de format : Set-Cookie: session_id=TON_ID_UNIQUE; Max-Age=3600; Path=/.

    Création du cas d'usage (Exigence du sujet) :
        Développer un script CGI ou une route C++ dédiée (ex: /login ou /profile) qui vérifie la présence du cookie.
        Si pas de cookie connu : Générer une nouvelle session, envoyer le header Set-Cookie et afficher une page de création/bienvenue.
        Si cookie reconnu : Récupérer la valeur associée dans la map du serveur et renvoyer une page personnalisée (ex: "Heureux de vous revoir").
