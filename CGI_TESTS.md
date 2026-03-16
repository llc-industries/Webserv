# CGI Test Suite

## Lancer tous les tests automatiquement

```bash
./webserv conf/test.conf &   # Terminal A : démarrer le serveur
bash www/cgi_test.sh localhost 8080  # Terminal B : lancer la suite
```

---

## Groupe 1 — GET / POST Basiques

### 1.1 GET PHP avec QUERY_STRING
* **Fichier :** `test.php`
* **Commande :** `curl -i "http://localhost:8080/test.php?user=anas&age=21"`
* **Comportement attendu :**
  - Code `200 OK`
  - `QUERY_STRING=user=anas&age=21` transmis au script
  - Les valeurs `anas` et `21` apparaissent dans la réponse

### 1.2 GET Python avec QUERY_STRING
* **Fichier :** `test.py`
* **Commande :** `curl -i "http://localhost:8080/test.py?lang=python&version=3"`
* **Comportement attendu :**
  - Code `200 OK`
  - La valeur `python` apparaît dans la réponse

### 1.3 POST PHP avec body
* **Fichier :** `test.php`
* **Commande :** `curl -i -X POST -d "Ceci est un test de body" http://localhost:8080/test.php`
* **Comportement attendu :**
  - Code `200 OK`
  - `CONTENT_LENGTH` et `CONTENT_TYPE` définis dans l'environnement
  - Le texte `"Ceci est un test de body"` lu via `stdin` apparaît dans la réponse

### 1.4 POST Python avec body
* **Fichier :** `test.py`
* **Commande :** `curl -i -X POST -d "hello from python" http://localhost:8080/test.py`
* **Comportement attendu :**
  - Code `200 OK`
  - Le texte `"hello from python"` apparaît dans la réponse

---

## Groupe 2 — Variables d'environnement CGI

### 2.1 Dump complet des variables CGI
* **Fichier :** `env.py`
* **Commande :** `curl -i "http://localhost:8080/env.py?foo=bar"`
* **Comportement attendu :**
  - `REQUEST_METHOD=GET`
  - `QUERY_STRING=foo=bar`
  - `GATEWAY_INTERFACE=CGI/1.1`
  - `SERVER_PROTOCOL=HTTP/1.1`
  - `SCRIPT_FILENAME=` chemin absolu vers `env.py`

### 2.2 Variables POST
* **Fichier :** `env.py`
* **Commande :** `curl -i -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "data=test" http://localhost:8080/env.py`
* **Comportement attendu :**
  - `REQUEST_METHOD=POST`
  - `CONTENT_TYPE=application/x-www-form-urlencoded`
  - `CONTENT_LENGTH=9` (longueur de `data=test`)

---

## Groupe 3 — Headers CGI retournés

### 3.1 Headers personnalisés (Set-Cookie, X-Custom)
* **Fichier :** `headers.py`
* **Commande :** `curl -i http://localhost:8080/headers.py`
* **Comportement attendu :**
  - Code `200 OK`
  - Header `Set-Cookie: session_id=abc123; Path=/` présent dans la réponse HTTP
  - Header `X-Custom-Header: hello` présent
* **Note :** Si ces headers sont absents, `parseCgiResponse()` strip les headers CGI — bug à corriger.

### 3.2 Headers avec séparateur LF-only (\n\n)
* **Fichier :** `lf_headers.py`
* **Commande :** `curl -i http://localhost:8080/lf_headers.py`
* **Comportement attendu :**
  - Code `200 OK`
  - Le body `"LF-only headers OK"` est bien reçu
  - Teste la robustesse de `parseCgiResponse()` avec `\n\n` au lieu de `\r\n\r\n`

---

## Groupe 4 — POST edge cases

### 4.1 POST avec body vide (Content-Length: 0)
* **Fichier :** `empty_post.py`
* **Commande :** `curl -i -X POST -H "Content-Length: 0" http://localhost:8080/empty_post.py`
* **Comportement attendu :**
  - Code `200 OK`
  - `"0 bytes"` dans la réponse
  - Le pipe CGI stdin est fermé immédiatement sans deadlock

### 4.2 POST avec 100KB de données
* **Fichier :** `big_post.py`
* **Commande :** `curl -i -X POST --data-binary "$(python3 -c "print('A'*102400,end='')")" http://localhost:8080/big_post.py`
* **Comportement attendu :**
  - Code `200 OK`
  - `"Received=102400 bytes"` dans la réponse
  - Teste que le body entier transite correctement par le pipe non-bloquant

### 4.3 POST avec 500KB de données
* **Fichier :** `big_post.py`
* **Commande :** `python3 -c "print('B'*512000,end='')" | curl -i -X POST --data-binary @- http://localhost:8080/big_post.py`
* **Comportement attendu :**
  - Code `200 OK` si `client_max_body_size` est suffisant (≥ 512000)
  - `413 Entity Too Large` si la limite est dépassée
  - `"Received=512000 bytes"` dans la réponse si OK

---

## Groupe 5 — Robustesse / Erreurs

### 5.1 Script inexistant
* **Commande :** `curl -i http://localhost:8080/nonexistent_script.py`
* **Comportement attendu :**
  - Code `404` ou `500`
  - Le serveur ne crash pas

### 5.2 Crash interne du script (Broken Pipe)
* **Fichier :** `crash.py`
* **Commande :** `curl -i -X POST -d "Grand Body" http://localhost:8080/crash.py`
* **Comportement attendu :**
  - Le script plante (division par zéro)
  - Le serveur **ne crash pas** (grâce à `signal(SIGPIPE, SIG_IGN)`)
  - Réponse `500` ou `502` reçue
  
### 5.3 Serveur vivant après crash
* **Commandes :**
  ```bash
  curl -i http://localhost:8080/crash.py
  curl -i http://localhost:8080/         # Doit répondre 200
  ```
* **Comportement attendu :** Le second curl retourne `200 OK`

---

## Groupe 6 — Concurrence non-bloquante

### 6.1 Sleep + GET simultané (preuve non-blocking)
* **Fichiers :** `sleep5.py`
* **Test :**
  1. Terminal A : `curl -i http://localhost:8080/sleep5.py` (attend ~5s)
  2. Terminal B (immédiatement) : `curl -i http://localhost:8080/`
* **Comportement attendu :**
  - Terminal B reçoit sa réponse en `< 1 seconde`
  - Terminal A reçoit sa réponse après ~5 secondes
  - *Preuve que epoll gère le CGI de manière asynchrone*

### 6.2 5 requêtes CGI simultanées
* **Commande :**
  ```bash
  for i in 1 2 3 4 5; do
    curl -s "http://localhost:8080/test.py?id=$i" &
  done
  wait
  ```
* **Comportement attendu :**
  - Toutes les requêtes reçoivent une réponse `200 OK`
  - Pas de deadlock, pas de timeout

---

## Groupe 7 — Timeout CGI

### 7.1 Boucle infinie — SIGKILL + 502
* **Fichiers :** `infinite.py` ou `infinite.php`
* **Commande :** `curl -i http://localhost:8080/infinite.py`
* **Comportement attendu :**
  - Le processus CGI tourne en boucle infinie
  - Le serveur reste réactif pour les autres clients
  - Après `CLIENT_TIMEOUT` (30s), le serveur envoie `SIGKILL` au processus
  - Réponse `502 Bad Gateway` reçue dans ≤ 35 secondes

### 7.2 Serveur vivant après timeout
* **Commandes :**
  ```bash
  curl -i http://localhost:8080/infinite.py   # Attend ~30s
  curl -i http://localhost:8080/              # Doit répondre 200
  ```
* **Comportement attendu :** Le second curl retourne `200 OK`

---

## Groupe 8 — Sortie volumineuse

### 8.1 Grande sortie CGI
* **Fichier :** `spam.php`
* **Commande :** `curl -i http://localhost:8080/spam.php | wc -c`
* **Comportement attendu :**
  - Code `200 OK`
  - Réponse complète reçue (> 100 000 lignes)
  - Pas de troncature, pas de timeout

### 8.2 Stress test avec Siege
* **Commande :** `siege -b -c 100 -t 10S http://localhost:8080/test.php`
* **Comportement attendu :**
  - `100%` de disponibilité
  - `0` transaction échouée
  - Pas de fuite de file descriptors (vérifier avec `lsof -p $(pgrep webserv) | wc -l`)

---

## Nouveaux scripts de test

| Script | Objectif |
|--------|----------|
| `env.py` | Dump de toutes les variables d'environnement CGI |
| `headers.py` | Retourne `Set-Cookie` + `X-Custom-Header` |
| `lf_headers.py` | Séparateur `\n\n` (LF only) au lieu de `\r\n\r\n` |
| `empty_post.py` | POST avec `Content-Length: 0` |
| `big_post.py` | POST avec body de 100KB / 500KB |
| `status.py` | Retourne `Status: 404 Not Found` (teste le respect du status CGI) |
| `cgi_test.sh` | Suite de test automatisée (22 assertions pass/fail) |
