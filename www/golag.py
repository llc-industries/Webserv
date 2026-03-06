#!/usr/bin/env python3

print("Content-Type: text/html\r\n\r\n", end="")
print("<html><body><h1>Je vais crasher...</h1></body></html>")

# Erreur fatale volontaire : Division par zéro
x = 1 / 0