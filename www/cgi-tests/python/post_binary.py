#!/usr/bin/env python3
import os
import sys

UPLOAD_DIR = "./www/upload"

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")

try:
    content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
except ValueError:
    content_length = 0

binary_body = sys.stdin.buffer.read(content_length)

# Sauvegarde du fichier
filename = os.environ.get("HTTP_X_FILENAME", "upload.bin")
filepath = os.path.join(UPLOAD_DIR, os.path.basename(filename))

os.makedirs(UPLOAD_DIR, exist_ok=True)
with open(filepath, "wb") as f:
    f.write(binary_body)

saved = len(binary_body)
color = "#a6e3a1" if saved == content_length else "#f38ba8"

print(f"""<!DOCTYPE html><html><head><meta charset="UTF-8">
<style>
    body {{ font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }}
    .box {{ background: #181825; padding: 20px; border: 1px solid {color}; border-radius: 6px; }}
    h1 {{ color: {color}; }}
    pre {{ background: #11111b; padding: 15px; border-left: 3px solid {color}; }}
</style></head><body><div class="box">
    <h1>Upload {'OK ✓' if saved == content_length else 'ERREUR ✗'}</h1>
    <p>Fichier : <code>{filepath}</code></p>
    <p>CONTENT_LENGTH : {content_length}</p>
    <p>Bytes écrits : {saved}</p>
    <pre>{repr(binary_body[:50])}</pre>
</div></body></html>""")