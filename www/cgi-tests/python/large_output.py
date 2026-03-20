#!/usr/bin/env python3
import sys

sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n\r\n")

html_start = """<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>large_output.py</title>
<style>
    body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }
    .box { background: #181825; padding: 20px; border: 1px solid #f38ba8; border-radius: 6px; }
    h1 { color: #f38ba8; padding-bottom: 10px; }
    ul { list-style-type: none; padding: 0; }
    li { color: #a6adc8; font-size: 0.8em; margin-bottom: 2px; }
</style></head><body>
<div class="box">
    <h1>Stress test</h1>
    <p>100 000 Lines generated</p>
</div>
<ul>
"""
sys.stdout.write(html_start)

line_content = "<li>" + "0123456789" * 10

for i in range(100000):
    sys.stdout.write(line_content)

sys.stdout.write("</ul></body></html>")
sys.stdout.flush()
