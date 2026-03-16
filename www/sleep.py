#!/usr/bin/env python3
import time
import sys
import os

time.sleep(10)

content_length = int(os.environ.get("CONTENT_LENGTH", 0))
if content_length > 0:
    sys.stdin.buffer.read(content_length)

print("Content-Type: text/html\r\n\r\n", end="")
print("<html><body><h1>Fini !</h1></body></html>")
