#!/usr/bin/env python3
import sys

sys.stdout.write("Content-Type: text/html; charset=utf-8\n\n") # Notice \n\n instead of \r\n\r\n

html = """<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>lf_separator.py</title>
<style>
    body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }
    .box { background: #181825; padding: 20px; border: 1px solid #a6e3a1; border-radius: 6px; }
    h1 { color: #a6e3a1; padding-bottom: 10px; }
    p { font-size: 1.1em; }
</style></head><body>
<div class="box">
    <h1>LF Separator: Success</h1>
    <p>Sent header = Content-Type: text/html; charset=utf-8\\n\\n</p>
</div></body></html>"""

sys.stdout.write(html)
sys.stdout.flush()
