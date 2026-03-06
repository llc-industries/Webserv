#!/usr/bin/env python3
import time
import sys

# Le CGI met du temps à calculer...
time.sleep(5)

print("Content-Type: text/html\r\n\r\n", end="")
print("<html><body><h1>😴 Je me réveille après 5 secondes !</h1></body></html>")