#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")

content_length = os.environ.get("CONTENT_LENGTH", "Undefined")

body = sys.stdin.read()

html = f"""<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>post_empty.py</title>
<style>
    body {{ font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }}
    .box {{ background: #181825; padding: 20px; border: 1px solid #f9e2af; border-radius: 6px; }}
    h1 {{ color: #f9e2af; padding-bottom: 10px; }}
</style></head><body>
<div class="box">
    <h1>Test POST: Empty Body</h1>
    <p>CONTENT_LENGTH: {content_length}</p>
    <p>Read body (should be empty): '{body}'</p>
    <p>If you can read this it's ok :)</p>
</div></body></html>"""

print(html)
