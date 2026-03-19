#!/usr/bin/env python3
import os, sys

cl_str = os.environ.get("CONTENT_LENGTH", "0")
cl = int(cl_str) if cl_str.isdigit() else 0
body = sys.stdin.read(cl)

print("Content-Type: text/plain")
print()
print("CONTENT_LENGTH=" + cl_str)
print("Received=" + str(len(body)) + " bytes")
if len(body) == cl:
    print("OK: full body received")
else:
    print("MISMATCH: expected " + str(cl) + " got " + str(len(body)))
