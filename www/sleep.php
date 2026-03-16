<?php
echo "Content-Type: text/html\r\n\r\n";

sleep(5);

echo "<html><body>";
echo "<h1>I slept 5 seconds</h1>";
echo "<p>Other request should be working during this time</p>";
echo "</body></html>";
?>
