#!/usr/bin/env python3
import time
import sys

# On envoie l'en-tête et on force l'écriture immédiate (flush)
print("Content-Type: text/html\r\n\r\n", end="", flush=True)

# Bienvenue au Goulag (Boucle infinie)
while True:
    time.sleep(1)