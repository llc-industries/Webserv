#!/usr/bin/env python3
import os
import sys

# Récupération de l'environnement
cookies_raw = os.environ.get("HTTP_COOKIE", "")
query = os.environ.get("QUERY_STRING", "")

# Préparation des headers CGI
headers = "Content-Type: text/html; charset=utf-8\r\n"

# Logique de session basée sur la query string
if "action=login" in query:
    headers += "Set-Cookie: session=42_logged_in; Path=/; Max-Age=3600\r\n"
elif "action=logout" in query:
    headers += "Set-Cookie: session=; Path=/; Max-Age=0\r\n"

headers += "\r\n" # Fin des headers
sys.stdout.write(headers)

# Détermination de l'état pour l'affichage HTML
status = "Déconnecté 🔴"
if "session=42_logged_in" in cookies_raw:
    status = "Connecté 🟢"

html = f"""<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>login.py</title>
<style>
    body {{ font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }}
    .box {{ background: #181825; padding: 20px; border: 1px solid #cba6f7; border-radius: 6px; }}
    h1 {{ color: #cba6f7; padding-bottom: 10px; }}
    a {{ color: #1e1e2e; background: #a6e3a1; padding: 5px 10px; text-decoration: none; border-radius: 3px; margin-right: 10px; }}
    a.red {{ background: #f38ba8; }}
    a.blue {{ background: #89b4fa; }}
</style></head><body>
<div class="box">
    <h1>Test Cookies & Session</h1>
    <p>Statut : <strong>{status}</strong></p>
    <p>Cookie reçu (HTTP_COOKIE) : <code>{cookies_raw if cookies_raw else "Aucun"}</code></p>
    <br>
    <a href="?action=login">Login</a>
    <a href="?action=logout" class="red">Logout</a>
    <a href="login.py" class="blue">Refresh</a>
</div></body></html>"""

sys.stdout.write(html)
sys.stdout.flush()
