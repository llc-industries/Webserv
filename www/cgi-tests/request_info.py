#!/usr/bin/env python3
import os
import sys

# La norme CGI impose d'envoyer les headers suivis d'une ligne vide (\r\n\r\n)
print("Content-Type: text/html\r\n\r\n", end="")

print("<html><body>")
print("<h1>Test CGI Python Réussi !</h1>")

method = os.environ.get("REQUEST_METHOD", "INCONNU")
print(f"<p><strong>Méthode :</strong> {method}</p>")
print(f"<p><strong>Query String :</strong> {os.environ.get('QUERY_STRING', '')}</p>")

if method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    if content_length > 0:
        # On lit exactement ce que le serveur C++ a écrit dans le pipe_in
        body = sys.stdin.read(content_length)
        print("<h3>Données POST :</h3>")
        print(f"<pre style='background:#eee; padding:10px;'>{body}</pre>")

print("</body></html>")