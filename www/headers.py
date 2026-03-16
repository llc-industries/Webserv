#!/usr/bin/env python3
print("Content-Type: text/html")
print("Set-Cookie: session_id=abc123; Path=/")
print("X-Custom-Header: hello")
print()
print("<h1>Headers test</h1>")
print("<p>Set-Cookie and X-Custom-Header were sent by CGI.</p>")
print("<p>Check the HTTP response headers with: curl -i http://localhost:8080/headers.py</p>")
