#!/usr/bin/python3
import os
import sys

# ── Environnement ──────────────────────────────────────────────────────────────
cookies_raw = os.environ.get("HTTP_COOKIE", "")
query       = os.environ.get("QUERY_STRING", "")
CGI_PATH    = "/cgi-tests/python/cookie_cart.py"

ITEMS = ["Pizza", "Burger", "Sushi", "Tacos"]

# ── Parse cookies ──────────────────────────────────────────────────────────────
cookies = {}
for p in cookies_raw.split("; "):
    if "=" in p:
        k, v = p.split("=", 1)
        cookies[k.strip()] = v.strip()

cart_cookie = cookies.get("cart", "")
cart = [i for i in cart_cookie.split(",") if i in ITEMS] if cart_cookie else []

# ── Parse query string ─────────────────────────────────────────────────────────
params = {}
for pair in query.split("&"):
    if "=" in pair:
        k, v = pair.split("=", 1)
        params[k.strip()] = v.strip()

action = params.get("action", "")
item   = params.get("item", "")

# ── Logique panier ─────────────────────────────────────────────────────────────
new_cookie_header = ""

if action == "add" and item in ITEMS and item not in cart:
    cart.append(item)
    new_cookie_header = "Set-Cookie: cart=" + ",".join(cart) + "; Path=/; Max-Age=3600"

elif action == "remove" and item in ITEMS and item in cart:
    cart.remove(item)
    val = ",".join(cart) if cart else ""
    new_cookie_header = "Set-Cookie: cart=" + val + "; Path=/; Max-Age=3600"

elif action == "clear":
    cart = []
    new_cookie_header = "Set-Cookie: cart=; Path=/; Max-Age=0"

# ── Style ──────────────────────────────────────────────────────────────────────
STYLE = """
    * { box-sizing: border-box; }
    body  { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; margin: 0; }
    .box  { background: #181825; padding: 30px; border: 1px solid #cba6f7; border-radius: 6px; max-width: 540px; }
    h1    { color: #cba6f7; margin-top: 0; padding-bottom: 10px; border-bottom: 1px solid #313244; }
    h2    { color: #a6adc8; font-size: 0.95em; margin: 20px 0 8px 0; text-transform: uppercase; letter-spacing: 1px; }
    p     { margin: 6px 0; font-size: 0.9em; }
    code  { background: #313244; padding: 2px 6px; border-radius: 3px; font-size: 0.88em; }
    .grid { display: flex; gap: 10px; flex-wrap: wrap; margin-bottom: 10px; }
    .btn  { color: #1e1e2e; padding: 7px 16px; text-decoration: none; border-radius: 3px;
            font-family: monospace; font-size: 0.95em; white-space: nowrap; }
    .green  { background: #a6e3a1; }
    .red    { background: #f38ba8; }
    .yellow { background: #f9e2af; }
    .blue   { background: #89b4fa; }
    .gray   { background: #45475a; color: #cdd6f4; }
    .cart-item { display: flex; align-items: center; justify-content: space-between;
                 background: #1e3a2f; border-left: 3px solid #a6e3a1;
                 padding: 6px 12px; border-radius: 3px; margin-bottom: 6px; }
    .cart-item.empty { background: #2a2a3d; border-left-color: #45475a; color: #6c7086; }
    .divider { border: none; border-top: 1px solid #313244; margin: 18px 0; }
    .debug  { background: #1e2a3a; border-left: 3px solid #89b4fa; padding: 8px 12px;
              border-radius: 3px; margin-top: 16px; font-size: 0.85em; }
"""

# ── HTML ───────────────────────────────────────────────────────────────────────
# Boutons ajouter
add_buttons = ""
for food in ITEMS:
    if food in cart:
        add_buttons += '<span class="btn gray">' + food + ' ✓</span>\n        '
    else:
        add_buttons += '<a class="btn green" href="' + CGI_PATH + '?action=add&item=' + food + '">' + food + '</a>\n        '

# Contenu du panier
if cart:
    cart_html = ""
    for food in cart:
        cart_html += '''<div class="cart-item">
            <span>''' + food + '''</span>
            <a class="btn red" href="''' + CGI_PATH + '?action=remove&item=' + food + '''">Retirer</a>
        </div>
        '''
    total_line = '<p style="margin-top:10px;">Total : <strong>' + str(len(cart)) + ' article' + ('s' if len(cart) > 1 else '') + '</strong></p>'
    clear_btn = '<a class="btn yellow" href="' + CGI_PATH + '?action=clear">Vider le panier</a>'
else:
    cart_html = '<div class="cart-item empty"><span>Panier vide</span></div>'
    total_line = ""
    clear_btn = ""

# ── Output ─────────────────────────────────────────────────────────────────────
sys.stdout.write("Content-Type: text/html\r\n")
if new_cookie_header:
    sys.stdout.write(new_cookie_header + "\r\n")
sys.stdout.write("\r\n")

sys.stdout.write("""<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8"><title>cookie_cart.py</title>
<style>""" + STYLE + """</style></head>
<body><div class="box">
    <h1>🛒 Cookie Cart</h1>

    <h2>Ajouter au panier</h2>
    <div class="grid">
        """ + add_buttons + """
    </div>

    <hr class="divider">

    <h2>Panier</h2>
    """ + cart_html + """
    """ + total_line + """
    """ + ('<br>' + clear_btn if clear_btn else '') + """

    <div class="debug">
        <strong>Debug</strong><br>
        HTTP_COOKIE : <code>""" + (cookies_raw if cookies_raw else "Aucun") + """</code><br>
        QUERY_STRING : <code>""" + (query if query else "Aucun") + """</code>
    </div>

    <br>
    <a class="btn blue" href="/">Accueil</a>
</div></body></html>""")

sys.stdout.flush()