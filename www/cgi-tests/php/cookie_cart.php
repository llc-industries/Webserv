<?php
// ── Environnement ──────────────────────────────────────────────────────────────
$cookies_raw = $_SERVER['HTTP_COOKIE'] ?? '';
$query       = $_SERVER['QUERY_STRING'] ?? '';
$CGI_PATH    = '/cgi-tests/php/cookie_cart.php';

$ITEMS = ['Pizza', 'Burger', 'Sushi', 'Tacos'];

// ── Parse cookies ──────────────────────────────────────────────────────────────
$cookies = [];
foreach (explode('; ', $cookies_raw) as $p) {
    if (strpos($p, '=') !== false) {
        [$k, $v] = explode('=', $p, 2);
        $cookies[trim($k)] = trim($v);
    }
}

$cart_cookie = $cookies['cart'] ?? '';
$cart = $cart_cookie
    ? array_values(array_filter(explode(',', $cart_cookie), fn($i) => in_array($i, $ITEMS)))
    : [];

// ── Parse query string ─────────────────────────────────────────────────────────
$params = [];
parse_str($query, $params);

$action = $params['action'] ?? '';
$item   = $params['item'] ?? '';

// ── Logique panier ─────────────────────────────────────────────────────────────
$new_cookie_header = '';

if ($action === 'add' && in_array($item, $ITEMS) && !in_array($item, $cart)) {
    $cart[] = $item;
    $new_cookie_header = 'Set-Cookie: cart=' . implode(',', $cart) . '; Path=/; Max-Age=3600';

} elseif ($action === 'remove' && in_array($item, $ITEMS) && in_array($item, $cart)) {
    $cart = array_values(array_filter($cart, fn($i) => $i !== $item));
    $new_cookie_header = 'Set-Cookie: cart=' . implode(',', $cart) . '; Path=/; Max-Age=3600';

} elseif ($action === 'clear') {
    $cart = [];
    $new_cookie_header = 'Set-Cookie: cart=; Path=/; Max-Age=0';
}

// ── Style ──────────────────────────────────────────────────────────────────────
$STYLE = '
    * { box-sizing: border-box; }
    body  { font-family: monospace; background: #1e1e2e; color: #cdd6f4; padding: 40px; margin: 0; }
    .box  { background: #181825; padding: 30px; border: 1px solid #cba6f7; border-radius: 6px; max-width: 540px; }
    h1    { color: #cba6f7; margin-top: 0; padding-bottom: 10px; border-bottom: 1px solid #313244; }
    h2    { color: #a6adc8; font-size: 0.95em; margin: 20px 0 8px 0; text-transform: uppercase; letter-spacing: 1px; }
    p     { margin: 6px 0; font-size: 0.9em; }
    code  { background: #313244; padding: 2px 6px; border-radius: 3px; font-size: 0.88em; }
    .grid { display: flex; gap: 10px; flex-wrap: wrap; margin-bottom: 10px; }
    .btn  { color: #1e1e2e; padding: 7px 16px; text-decoration: none; border-radius: 3px;
            font-family: monospace; font-size: 0.95em; white-space: nowrap; }
    .green  { background: #a6e3a1; }
    .red    { background: #f38ba8; }
    .yellow { background: #f9e2af; }
    .blue   { background: #89b4fa; }
    .gray   { background: #45475a; color: #cdd6f4; }
    .cart-item { display: flex; align-items: center; justify-content: space-between;
                 background: #1e3a2f; border-left: 3px solid #a6e3a1;
                 padding: 6px 12px; border-radius: 3px; margin-bottom: 6px; }
    .cart-item.empty { background: #2a2a3d; border-left-color: #45475a; color: #6c7086; }
    .divider { border: none; border-top: 1px solid #313244; margin: 18px 0; }
    .debug  { background: #1e2a3a; border-left: 3px solid #89b4fa; padding: 8px 12px;
              border-radius: 3px; margin-top: 16px; font-size: 0.85em; }
';

// ── HTML ───────────────────────────────────────────────────────────────────────
// Boutons ajouter
$add_buttons = '';
foreach ($ITEMS as $food) {
    if (in_array($food, $cart)) {
        $add_buttons .= '<span class="btn gray">' . $food . ' ✓</span>' . "\n        ";
    } else {
        $add_buttons .= '<a class="btn green" href="' . $CGI_PATH . '?action=add&item=' . $food . '">' . $food . '</a>' . "\n        ";
    }
}

// Contenu du panier
if ($cart) {
    $cart_html = '';
    foreach ($cart as $food) {
        $cart_html .= '<div class="cart-item">
            <span>' . htmlspecialchars($food) . '</span>
            <a class="btn red" href="' . $CGI_PATH . '?action=remove&item=' . urlencode($food) . '">Retirer</a>
        </div>
        ';
    }
    $count = count($cart);
    $total_line = '<p style="margin-top:10px;">Total : <strong>' . $count . ' article' . ($count > 1 ? 's' : '') . '</strong></p>';
    $clear_btn  = '<a class="btn yellow" href="' . $CGI_PATH . '?action=clear">Vider le panier</a>';
} else {
    $cart_html  = '<div class="cart-item empty"><span>Panier vide</span></div>';
    $total_line = '';
    $clear_btn  = '';
}

// ── Output ─────────────────────────────────────────────────────────────────────
if ($new_cookie_header) {
    header($new_cookie_header);
}
header('Content-Type: text/html');

echo '<!DOCTYPE html>
<html lang="fr"><head><meta charset="UTF-8"><title>cookie_cart.php</title>
<style>' . $STYLE . '</style></head>
<body><div class="box">
    <h1>🛒 Cookie Cart</h1>

    <h2>Ajouter au panier</h2>
    <div class="grid">
        ' . $add_buttons . '
    </div>

    <hr class="divider">

    <h2>Panier</h2>
    ' . $cart_html . '
    ' . $total_line . '
    ' . ($clear_btn ? '<br>' . $clear_btn : '') . '

    <div class="debug">
        <strong>Debug</strong><br>
        HTTP_COOKIE : <code>' . htmlspecialchars($cookies_raw ?: 'Aucun') . '</code><br>
        QUERY_STRING : <code>' . htmlspecialchars($query ?: 'Aucun') . '</code>
    </div>

    <br>
    <a class="btn blue" href="/">Accueil</a>
</div></body></html>';
