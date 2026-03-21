<?php
header("Content-Type: text/html; charset=utf-8");

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>large_output.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo '.box { background: #181825; padding: 20px; border: 1px solid #f38ba8; border-radius: 6px; }';
echo 'h1 { color: #f38ba8; padding-bottom: 10px; }';
echo 'ul { list-style-type: none; padding: 0; }';
echo 'li { color: #a6adc8; font-size: 0.8em; margin-bottom: 2px; }';
echo '</style></head><body>';
echo '<div class="box">';
echo '    <h1>Stress test</h1>';
echo '    <p>100 000 Lines generated</p>';
echo '</div>';
echo '<ul>';

$line = '<li>' . str_repeat('0123456789', 10);
for ($i = 0; $i < 100000; $i++) {
    echo $line;
}

echo '</ul></body></html>';
