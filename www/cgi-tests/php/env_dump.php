<?php
header("Content-Type: text/html; charset=utf-8");

$vars = array_merge($_SERVER, getenv() ?: []);
ksort($vars);

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>env_dump.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo '.box { background: #181825; padding: 20px; border: 1px solid #313244; border-radius: 6px; }';
echo 'h1 { color: #89b4fa; border-bottom: 1px solid #313244; padding-bottom: 10px; }';
echo 'li { margin-bottom: 5px; }';
echo 'strong { color: #a6e3a1; }';
echo '</style></head><body>';
echo '<div class="box"><h1>Env dump:</h1><ul>';

foreach ($vars as $k => $v) {
    echo "<li><strong>" . htmlspecialchars($k) . "</strong> = " . htmlspecialchars((string)$v) . "</li>";
}

echo '</ul></div></body></html>';
