#!/usr/bin/python3
import os
import sys

# 1. Récupération des cookies envoyés par VOTRE serveur via l'env
cookies_raw = os.environ.get("HTTP_COOKIE", "")
cookies = {}
if cookies_raw:
    pairs = cookies_raw.split("; ")
    for p in pairs:
        if "=" in p:
            k, v = p.split("=", 1)
            cookies[k] = v

# 2. Logique de vérification
user_session = cookies.get("session_id")

print("Content-Type: text/html")

if user_session == "42_is_awesome":
    # ÉTAT : DÉJÀ CONNECTÉ
    print("\r\n") # Fin des headers
    print("<h1>Bienvenue, etudiant de 42 !</h1>")
    print("<p>Votre session_id est bien reconnu via le cookie.</p>")
    print("<a href='/'>Retour a l'accueil</a>")

else:
    # ÉTAT : FORMULAIRE OU TENTATIVE DE CONNEXION
    # On simule un check de login (en réel on lirait le corps du POST)
    # Ici, pour le test, on va dire que si on ajoute ?user=admin dans l'URL, on connecte
    query = os.environ.get("QUERY_STRING", "")
    
    if "user=admin" in query:
        # On demande au serveur de poser le cookie
        print("Set-Cookie: session_id=42_is_awesome; Path=/; HttpOnly")
        print("\r\n")
        print("<h1>Connexion reussie !</h1>")
        print("<p>Le cookie vient d'etre pose. <a href='/login.py'>Actualisez la page</a>.</p>")
    else:
        print("\r\n")
        print("<h1>Page de Login</h1>")
        print("<form action='/login.py' method='GET'>")
        print("User: <input type='text' name='user' value='admin'>")
        print("<input type='submit' value='Se connecter'>")
        print("</form>")