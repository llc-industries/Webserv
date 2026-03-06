<?php
echo "<!DOCTYPE html><html><head><title>Test PHP CGI</title></head><body>";
echo "<h1>🚀 Test CGI PHP Réussi !</h1>";

echo "<h3>Informations sur la Requête :</h3>";
echo "<ul>";
echo "<li><strong>Méthode :</strong> " . (isset($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : 'Non définie') . "</li>";
echo "<li><strong>Query String :</strong> " . (isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : 'Aucune') . "</li>";

// On vérifie si la clé existe avant de l'afficher !
if (isset($_SERVER['CONTENT_LENGTH'])) {
    echo "<li><strong>Content Length :</strong> " . $_SERVER['CONTENT_LENGTH'] . "</li>";
} else {
    echo "<li><strong>Content Length :</strong> (Pas de Body)</li>";
}
echo "</ul>";

if (isset($_SERVER['REQUEST_METHOD']) && $_SERVER['REQUEST_METHOD'] === 'POST') {
    $body = file_get_contents('php://input');
    echo "<h3>Données POST reçues :</h3>";
    echo "<pre style='background:#222; color:#0f0; padding:10px;'>" . htmlspecialchars($body) . "</pre>";
}

echo "</body></html>";
?>