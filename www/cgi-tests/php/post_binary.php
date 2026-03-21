<?php
$content_length = (int)($_SERVER['CONTENT_LENGTH'] ?? 0);

// multipart/form-data: php-cgi parses the body automatically → php://input is empty
// → read the uploaded file from $_FILES instead
if (isset($_FILES['fileToUpload']['tmp_name']) && $_FILES['fileToUpload']['error'] === UPLOAD_ERR_OK) {
    $binary_body = file_get_contents($_FILES['fileToUpload']['tmp_name']) ?: '';
} else {
    $binary_body = file_get_contents('php://input') ?: '';
}
$actual_length = strlen($binary_body);

$preview = repr_bytes(substr($binary_body, 0, 50));

function repr_bytes(string $s): string {
    $out = "b'";
    for ($i = 0; $i < strlen($s); $i++) {
        $o = ord($s[$i]);
        if ($o === ord('\\')) $out .= '\\\\';
        elseif ($o === ord("'"))  $out .= "\\'";
        elseif ($o >= 32 && $o < 127) $out .= $s[$i];
        else $out .= sprintf('\\x%02x', $o);
    }
    return $out . "'";
}

$color = ($actual_length === $content_length) ? '#a6e3a1' : '#f38ba8';

header("Content-Type: text/html; charset=utf-8");

echo '<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>upload_binary.php</title>';
echo '<style>';
echo 'body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; }';
echo ".box { background: #181825; padding: 20px; border: 1px solid $color; border-radius: 6px; }";
echo "h1 { color: $color; padding-bottom: 10px; }";
echo "pre { background: #11111b; padding: 15px; border-left: 3px solid $color; overflow-x: auto; word-wrap: break-word; }";
echo '</style></head><body>';
echo '<div class="box">';
echo '<h1>Test POST: Binary Safety</h1>';
echo '<p>CONTENT_LENGTH: ' . $content_length . '</p>';
echo '<p>Raw bytes read on stdin: ' . $actual_length . '</p>';
echo '<p>First 50 bytes preview: </p>';
echo '<pre>' . htmlspecialchars($preview, ENT_QUOTES, 'UTF-8') . '</pre>';
echo '</div></body></html>';
