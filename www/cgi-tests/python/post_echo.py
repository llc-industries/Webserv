#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")

try:
    content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
except ValueError:
    content_length = 0

body = sys.stdin.read(content_length)

html = f"""<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>post_echo.py</title>
<style>
    body {{ font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }}
    .box {{ background: #181825; padding: 20px; border: 1px solid #89b4fa; border-radius: 6px; }}
    h1 {{ color: #89b4fa; padding-bottom: 10px; }}
    pre {{ background: #11111b; padding: 15px; border-left: 3px solid #89b4fa; overflow-x: auto; }}
</style></head><body>
<div class="box">
    <h1>Test POST: Echo</h1>
    <p>CONTENT_LENGTH: {content_length}</p>
    <p>Stdin bytes read: {len(body)}</p>
    <pre>{body if body else "Empty"}</pre>
</div></body></html>"""

print(html)
