#!/usr/bin/env python3
import sys

# Use LF-only line endings (\n instead of \r\n)
# Tests parseCgiResponse() robustness with \n\n separator
sys.stdout.write("Content-Type: text/html\n")
sys.stdout.write("\n")
sys.stdout.write("<h1>LF-only headers OK</h1>\n")
sys.stdout.write("<p>This response used LF-only (\\n\\n) header separator</p>\n")
sys.stdout.flush()
