#!/usr/bin/env python3
import http.cookies
import os
import uuid
import datetime

def get_expiry(days=7):
    exp = datetime.datetime.utcnow() + datetime.timedelta(days=days)
    return exp.strftime("%a, %d %b %Y %H:%M:%S GMT")

def send_headers(cookie_header):
    print("Content-Type: text/html\r")
    if cookie_header:
        print(cookie_header, end="")
    print("\r")

def render_page(cookies_in, new_session=None):
    print("<html><head><style>")
    print("body{font-family:monospace;background:#1e1e2e;color:#cdd6f4;padding:2rem;}")
    print(".box{background:#313244;border-radius:8px;padding:1.5rem;max-width:600px;}")
    print(".tag{color:#a6e3a1;} .val{color:#f9e2af;} .new{color:#89b4fa;font-style:italic;}")
    print(".label{color:#6c7086;font-size:.85rem;margin-bottom:.5rem;}")
    print("</style></head><body><div class='box'>")
    print("<h2>🍪 Cookie Inspector</h2>")

    if new_session:
        print(f"<p class='new'>✦ Nouveau cookie posé → recharge la page pour le voir</p>")

    print("<p class='label'>Cookies reçus :</p>")
    if cookies_in:
        for key, val in cookies_in.items():
            print(f"<p><span class='tag'>{key}</span> = <span class='val'>{val.value}</span></p>")
    else:
        print("<p>Aucun cookie présent.</p>")

    print("</div></body></html>")

cookie_header = os.environ.get("HTTP_COOKIE", "")
cookies_in = http.cookies.SimpleCookie(cookie_header)

new_session = None
set_cookie_header = ""

if "session_id" not in cookies_in:
    new_session = str(uuid.uuid4())
    exp = get_expiry(days=7)
    set_cookie_header = (
        f"Set-Cookie: session_id={new_session}; "
        f"Path=/; Expires={exp}; HttpOnly; SameSite=Lax\r\n"
    )

send_headers(set_cookie_header)
render_page(cookies_in, new_session)