#!/usr/bin/env python3
import os, sys

cl_str = os.environ.get("CONTENT_LENGTH", "0")
cl = int(cl_str) if cl_str.isdigit() else 0
body = sys.stdin.read(cl) if cl > 0 else ""

print("Content-Type: text/html")
print()
print("<h1>Empty POST test</h1>")
print("<p>CONTENT_LENGTH=" + cl_str + "</p>")
print("<p>Body received: " + str(len(body)) + " bytes</p>")
if len(body) == 0:
    print("<p>OK: body is empty as expected</p>")
else:
    print("<p>UNEXPECTED: body should be empty</p>")
