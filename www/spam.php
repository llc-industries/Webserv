<?php
echo "Content-Type: text/html\r\n\r\n";

echo "<html><body>";
echo "<h1>SPAM TEST (100 000 lignes)</h1>";

for ($i = 0; $i < 100000; $i++) {
    echo "<p>Ligne numéro $i : Webserv est vraiment un super projet quand ça marche.</p>\n";
}

echo "</body></html>";
?>