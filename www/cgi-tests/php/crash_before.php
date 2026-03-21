<?php
posix_kill(getmypid(), SIGSEGV); // OS-level crash before any output → empty stdout → 500

header("Content-Type: text/html; charset=utf-8");
echo "<p>Can't see me ;)</p>";
