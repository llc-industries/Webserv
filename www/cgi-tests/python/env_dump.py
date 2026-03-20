#!/usr/bin/env python3
import os

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="")

html = """<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>env_dump.py</title>
<style>
    body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }
    .box { background: #181825; padding: 20px; border: 1px solid #313244; border-radius: 6px; }
    h1 { color: #89b4fa; border-bottom: 1px solid #313244; padding-bottom: 10px; }
    li { margin-bottom: 5px; }
    strong { color: #a6e3a1; }
</style></head><body>
<div class="box"><h1>Env dump:</h1><ul>"""

for k, v in sorted(os.environ.items()):
    html += f"<li><strong>{k}</strong> = {v}</li>"

html += "</ul></div></body></html>"
print(html)
