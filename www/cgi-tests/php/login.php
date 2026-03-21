<?php
// Redirect to login_save.php (main entry point for the session test)
fwrite(STDOUT, "HTTP/1.1 302 Found\r\nLocation: /cgi-tests/php/login_save.php\r\nContent-Type: text/html\r\n\r\n");
