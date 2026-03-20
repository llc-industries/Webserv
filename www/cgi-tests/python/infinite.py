#!/usr/bin/env python3
import time
import sys

print("infinite.py: Infinite loop is running... Webserv kill process after 30sec + send 502", flush=True, file=sys.stderr)

while True:
    time.sleep(1)
