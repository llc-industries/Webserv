#!/usr/bin/env python3
import sys
import os

body = sys.stdin.buffer.read()

with open("www/upload/chunk_result.bin", "wb") as f:
    f.write(body)

print("Content-Type: text/html\r\n\r\n", end="")
print(f"<html><body><h1>Fini ! Recu {len(body)} octets.</h1></body></html>")
