<?php
header("Content-Type: text/html; charset=utf-8");

$content_length = (int)($_SERVER['CONTENT_LENGTH'] ?? 0);
$body = file_get_contents('php://input') ?: '';
$actual_length = strlen($body);

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>post_echo.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo '.box { background: #181825; padding: 20px; border: 1px solid #89b4fa; border-radius: 6px; }';
echo 'h1 { color: #89b4fa; padding-bottom: 10px; }';
echo 'pre { background: #11111b; padding: 15px; border-left: 3px solid #89b4fa; overflow-x: auto; }';
echo '</style></head><body>';
echo '<div class="box">';
echo '<h1>Test POST: Echo</h1>';
echo '<p>CONTENT_LENGTH: ' . $content_length . '</p>';
echo '<p>Stdin bytes read: ' . $actual_length . '</p>';
echo '<pre>' . ($body !== '' ? htmlspecialchars($body) : 'Empty') . '</pre>';
echo '</div></body></html>';
