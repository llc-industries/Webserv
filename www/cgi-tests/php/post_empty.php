<?php
header("Content-Type: text/html; charset=utf-8");

$content_length = $_SERVER['CONTENT_LENGTH'] ?? 'Undefined';
$body = file_get_contents('php://input') ?: '';

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>post_empty.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo '.box { background: #181825; padding: 20px; border: 1px solid #f9e2af; border-radius: 6px; }';
echo 'h1 { color: #f9e2af; padding-bottom: 10px; }';
echo '</style></head><body>';
echo '<div class="box">';
echo '<h1>Test POST: Empty Body</h1>';
echo '<p>CONTENT_LENGTH: ' . htmlspecialchars((string)$content_length) . '</p>';
echo "<p>Read body (should be empty): '" . htmlspecialchars($body) . "'</p>";
echo "<p>If you can read this it's ok :)</p>";
echo '</div></body></html>';
