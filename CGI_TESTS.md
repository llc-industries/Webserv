## 1. Test Basique (GET)
* **Fichiers :** `test.php` ou `test.py`
* **Commande :** `curl -i "http://localhost:8080/test.php?user=anas&age=21"`
* **Comportement Attendu :**
  - Code `200 OK`.
  - Le serveur doit extraire `user=anas&age=21` et le passer via la variable d'environnement `QUERY_STRING`.
  - La page renvoyée doit afficher ces valeurs.

## 2. Test avec Body (POST)
* **Fichiers :** `test.php` ou `test.py`
* **Commande :** `curl -i -X POST -d "Ceci est un test de body" http://localhost:8080/test.php`
* **Comportement Attendu :**
  - Code `200 OK`.
  - Les variables `CONTENT_LENGTH` et `CONTENT_TYPE` doivent être définies.
  - Le texte "Ceci est un test de body" lu via `stdin` doit être affiché dans la réponse.

## 3. Test de Concurrence Non-Bloquante (Sleep)
* **Fichiers :** `sleep5.py` ou `sleep.php`
* **Test :**
  1. Lancer `curl -i http://localhost:8080/sleep5.py` dans un Terminal A.
  2. IMMÉDIATEMENT, lancer `curl -i http://localhost:8080/` dans un Terminal B.
* **Comportement Attendu :**
  - Le Terminal B doit recevoir sa réponse HTML **immédiatement**, sans attendre le Terminal A.
  - Le Terminal A recevra sa réponse après le délai prévu (ex: 5 secondes).
  - *Preuve absolue que votre epoll gère le CGI de manière asynchrone.*

## 4. Test du Crash Interne (Broken Pipe)
* **Fichier :** `crash.py`
* **Commande :** `curl -i -X POST -d "Grand Body" http://localhost:8080/crash.py`
* **Comportement Attendu :**
  - Le script plante instantanément à cause d'une erreur interne (division par zéro, erreur de syntaxe).
  - Le serveur **NE DOIT PAS CRASHER** (Grâce au `signal(SIGPIPE, SIG_IGN)` dans le `main.cpp`).
  - Le serveur renvoie une page (même vide ou idéaleement une erreur `502/500`) et continue de fonctionner.

## 5. Le "Goulag" (Boucle Infinie & Timeout)
* **Fichiers :** `golag.py` ou `infinite.php`
* **Commande :** `curl -i http://localhost:8080/golag.py`
* **Comportement Attendu :**
  - Le processus enfant CGI va s'enfermer dans le Goulag (boucle infinie, 100% CPU).
  - Le serveur reste réactif pour les autres clients.
  - Le serveur doit détecter après X secondes (ex: 10s) que le script est trop long. Il doit tuer le processus avec `kill(pid, SIGKILL)`.
  - Le serveur renvoie une erreur `504 Gateway Timeout` ou `500 Internal Server Error`.

## 6. Test de Stress / Spam
* **Fichiers :** `spam.php` (ou utilisation de Siege)
* **Commande :** `siege -b -c 100 http://localhost:8080/test.php`
* **Comportement Attendu :**
  - Le serveur encaisse la charge sans fuite de mémoire ou de File Descriptors.
  - 100% de disponibilité requise avec 0 transaction échouée.