<?php
header("Content-Type: text/html; charset=utf-8");
echo "<p>Now crashing :(</p>";
ob_flush();
flush();

intdiv(1, 0); // DivisionByZeroError after partial output

echo "<p>Can't see me ;)</p>";
