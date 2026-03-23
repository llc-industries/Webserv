#!/usr/bin/env python3
import http.cookies
import os

# Lire les cookies entrants
cookie_header = os.environ.get("HTTP_COOKIE", "")
cookies_in = http.cookies.SimpleCookie(cookie_header)

# Poser un nouveau cookie si pas déjà là
set_cookie = ""
if "test_session" not in cookies_in:
    set_cookie = "Set-Cookie: test_session=abc123; Path=/\r\n"

print("Content-Type: text/html\r")
if set_cookie:
    print(set_cookie, end="")
print("\r")

print("<h2>Cookies reçus :</h2>")
if cookies_in:
    for key, val in cookies_in.items():
        print(f"<p>{key} = {val.value}</p>")
else:
    print("<p>Aucun cookie — <b>recharge la page</b>, le cookie vient d'être posé.</p>")