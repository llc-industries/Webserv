#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")

try:
    content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
except ValueError:
    content_length = 0

binary_body = sys.stdin.buffer.read(content_length)
actual_length = len(binary_body)

preview = repr(binary_body[:50])

color = "#a6e3a1" if actual_length == content_length else "#f38ba8"

html = f"""<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>upload_binary.py</title>
<style>
    body {{ font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }}
    .box {{ background: #181825; padding: 20px; border: 1px solid {color}; border-radius: 6px; }}
    h1 {{ color: {color}; padding-bottom: 10px; }}
    pre {{ background: #11111b; padding: 15px; border-left: 3px solid {color}; overflow-x: auto; word-wrap: break-word; }}
</style></head><body>
<div class="box">
    <h1>Test POST: Binary Safety</h1>
    <p>CONTENT_LENGTH: {content_length}</p>
    <p>Raw bytes read on stdin: {actual_length}</p>
    <p>First 5- bytes preview: </p>
    <pre>{preview}</pre>
</div></body></html>"""

print(html)
