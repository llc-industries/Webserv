<?php
// php-cgi always outputs \r\n\r\n as header separator.
// This test verifies the response is received correctly (php-cgi manages the separator).
header("Content-Type: text/html; charset=utf-8");

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>lf_separator.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo '.box { background: #181825; padding: 20px; border: 1px solid #a6e3a1; border-radius: 6px; }';
echo 'h1 { color: #a6e3a1; padding-bottom: 10px; }';
echo 'p { font-size: 1.1em; }';
echo '.note { color: #a6adc8; font-size: 0.9em; margin-top: 10px; }';
echo '</style></head><body>';
echo '<div class="box">';
echo '<h1>Header Separator: Success</h1>';
echo '<p>php-cgi always uses \\r\\n\\r\\n as header separator (managed internally).</p>';
echo '<p class="note">The \\n\\n edge case is N/A for PHP CGI — php-cgi handles headers before script output.</p>';
echo '</div></body></html>';
