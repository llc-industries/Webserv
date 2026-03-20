#!/usr/bin/env python3

print("Content-Type: text/html; charset=utf-8\r\n\r\n", end="", flush=True)
print("<p>Now crashing :(</p>", flush=True)

x = 1 / 0 # Crash
print("<p>Cant't see me ;)</p>", flush=True)
