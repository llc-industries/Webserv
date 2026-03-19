#!/bin/bash
# ============================================================
# CGI Test Suite for Webserv
# Usage: bash tests/cgi_test.sh [host] [port]
# Default: localhost 8080
# ============================================================

HOST="${1:-localhost}"
PORT="${2:-8080}"
BASE="http://${HOST}:${PORT}"
PASS=0
FAIL=0

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

pass() { echo -e "${GREEN}[PASS]${NC} $1"; ((PASS++)); }
fail() { echo -e "${RED}[FAIL]${NC} $1"; ((FAIL++)); }
section() { echo -e "\n${YELLOW}--- $1 ---${NC}"; }

check() {
    local label="$1"
    local response="$2"
    local expected="$3"
    if echo "$response" | grep -q "$expected"; then
        pass "$label"
    else
        fail "$label (expected: '$expected')"
        echo "       Response was: $(echo "$response" | head -5)"
    fi
}

check_status() {
    local label="$1"
    local response="$2"
    local expected_code="$3"
    if echo "$response" | grep -q "HTTP/1.1 ${expected_code}"; then
        pass "$label"
    else
        fail "$label (expected HTTP $expected_code)"
        echo "       Response: $(echo "$response" | head -1)"
    fi
}

# ============================================================
section "Groupe 1 — GET / POST Basiques"
# ============================================================

# T01 — GET PHP avec QUERY_STRING
R=$(curl -si "${BASE}/cgi-tests/test.php?user=anas&age=21" --max-time 5)
check_status "T01 GET PHP status 200" "$R" "200"
check        "T01 GET PHP QUERY_STRING contient 'anas'" "$R" "anas"

# T02 — GET Python avec QUERY_STRING
R=$(curl -si "${BASE}/cgi-tests/test.py?lang=python&version=3" --max-time 5)
check_status "T02 GET Python status 200" "$R" "200"
check        "T02 GET Python QUERY_STRING contient 'python'" "$R" "python"

# T03 — POST PHP avec body
R=$(curl -si -X POST -d "hello from test" "${BASE}/cgi-tests/test.php" --max-time 5)
check_status "T03 POST PHP status 200" "$R" "200"
check        "T03 POST PHP body lu par le script" "$R" "hello from test"

# T04 — POST Python avec body
R=$(curl -si -X POST -d "hello from python" "${BASE}/cgi-tests/test.py" --max-time 5)
check_status "T04 POST Python status 200" "$R" "200"
check        "T04 POST Python body lu par le script" "$R" "hello from python"

# ============================================================
section "Groupe 2 — Variables d'environnement CGI"
# ============================================================

# T05 — REQUEST_METHOD=GET
R=$(curl -si "${BASE}/cgi-tests/env.py" --max-time 5)
check "T05 REQUEST_METHOD=GET" "$R" "REQUEST_METHOD=GET"

# T06 — POST : CONTENT_TYPE et CONTENT_LENGTH présents
R=$(curl -si -X POST -H "Content-Type: application/x-www-form-urlencoded" \
    -d "data=test" "${BASE}/cgi-tests/env.py" --max-time 5)
check "T06 CONTENT_TYPE défini (POST)" "$R" "CONTENT_TYPE="
check "T06 CONTENT_LENGTH défini (POST)" "$R" "CONTENT_LENGTH="

# T07 — GATEWAY_INTERFACE=CGI/1.1
R=$(curl -si "${BASE}/cgi-tests/env.py" --max-time 5)
check "T07 GATEWAY_INTERFACE=CGI/1.1" "$R" "GATEWAY_INTERFACE=CGI/1.1"

# T08 — SERVER_PROTOCOL=HTTP/1.1
check "T08 SERVER_PROTOCOL=HTTP/1.1" "$R" "SERVER_PROTOCOL=HTTP/1.1"

# T09 — QUERY_STRING transmis
R=$(curl -si "${BASE}/cgi-tests/env.py?foo=bar&baz=42" --max-time 5)
check "T09 QUERY_STRING=foo=bar&baz=42" "$R" "QUERY_STRING=foo=bar"

# ============================================================
section "Groupe 3 — Headers CGI retournés"
# ============================================================

# T10 — headers.py : Set-Cookie transmis dans la réponse HTTP
R=$(curl -si "${BASE}/cgi-tests/headers.py" --max-time 5)
check_status "T10 headers.py status 200" "$R" "200"
check        "T10 Set-Cookie transmis" "$R" "Set-Cookie"

# T11 — lf_headers.py : réponse valide avec \n\n separateur
R=$(curl -si "${BASE}/cgi-tests/lf_headers.py" --max-time 5)
check_status "T11 lf_headers.py status 200" "$R" "200"
check        "T11 body reçu après \\n\\n" "$R" "LF-only headers OK"

# ============================================================
section "Groupe 4 — POST edge cases"
# ============================================================

# T12 — POST avec body vide (Content-Length: 0)
R=$(curl -si -X POST -H "Content-Length: 0" "${BASE}/cgi-tests/empty_post.py" --max-time 5)
check_status "T12 empty POST status 200" "$R" "200"
check        "T12 body vide détecté (0 bytes)" "$R" "0 bytes"

# T13 — POST avec 100KB
DATA=$(python3 -c "print('A' * 102400, end='')")
R=$(curl -si -X POST --data-binary "$DATA" "${BASE}/cgi-tests/big_post.py" --max-time 10)
check_status "T13 big POST 100KB status 200" "$R" "200"
check        "T13 big POST 100KB reçu entier" "$R" "102400 bytes"

# T14 — POST avec 500KB
DATA=$(python3 -c "print('B' * 512000, end='')")
R=$(curl -si -X POST --data-binary "$DATA" "${BASE}/cgi-tests/big_post.py" --max-time 10)
check_status "T14 big POST 500KB status 200" "$R" "200"
check        "T14 big POST 500KB reçu entier" "$R" "512000 bytes"

# ============================================================
section "Groupe 5 — Robustesse / Erreurs"
# ============================================================

# T15 — Script inexistant -> 404 ou 500
R=$(curl -si "${BASE}/cgi-tests/nonexistent_script.py" --max-time 5)
if echo "$R" | grep -qE "HTTP/1.1 (404|500)"; then
    pass "T15 Script inexistant -> 4xx/5xx"
else
    fail "T15 Script inexistant (attendu 404 ou 500)"
    echo "       Response: $(echo "$R" | head -1)"
fi

# T16 — crash.py : serveur ne crash pas
R=$(curl -si -X POST -d "test body" "${BASE}/cgi-tests/crash.py" --max-time 5)
if echo "$R" | grep -qE "HTTP/1.1 (200|500|502)"; then
    pass "T16 crash.py -> réponse reçue (serveur vivant)"
else
    fail "T16 crash.py -> pas de réponse HTTP"
fi

# T17 — Après crash.py, serveur toujours vivant
R=$(curl -si "${BASE}/" --max-time 5)
check_status "T17 Serveur vivant après crash.py" "$R" "200"

# ============================================================
section "Groupe 6 — Concurrence non-bloquante"
# ============================================================

# T18 — sleep5.py + GET simultané : GET doit répondre avant que sleep finisse
START=$(date +%s)
curl -s "${BASE}/cgi-tests/sleep5.py" --max-time 15 > /tmp/cgi_sleep_result.txt &
SLEEP_PID=$!
sleep 0.3
R=$(curl -si "${BASE}/" --max-time 5)
END=$(date +%s)
ELAPSED=$((END - START))

if [ $ELAPSED -lt 4 ] && echo "$R" | grep -q "HTTP/1.1 200"; then
    pass "T18 Non-blocking: GET répond pendant sleep5.py (${ELAPSED}s)"
else
    fail "T18 Non-blocking: GET a bloqué ou échoué (${ELAPSED}s)"
fi
wait $SLEEP_PID 2>/dev/null

# T19 — 5 requêtes CGI simultanées -> toutes répondent
PIDS=""
for i in 1 2 3 4 5; do
    curl -s "${BASE}/cgi-tests/test.py?id=$i" --max-time 10 > /tmp/cgi_concurrent_$i.txt &
    PIDS="$PIDS $!"
done
ALL_OK=true
for pid in $PIDS; do
    wait $pid
done
for i in 1 2 3 4 5; do
    if ! grep -q "id=$i" /tmp/cgi_concurrent_$i.txt 2>/dev/null; then
        if ! grep -q "200" /tmp/cgi_concurrent_$i.txt 2>/dev/null; then
            ALL_OK=false
        fi
    fi
done
rm -f /tmp/cgi_concurrent_*.txt
if $ALL_OK; then
    pass "T19 5 requêtes CGI simultanées -> toutes OK"
else
    fail "T19 5 requêtes CGI simultanées -> certaines ont échoué"
fi

# ============================================================
section "Groupe 7 — Timeout CGI"
# ============================================================

# T20 — infinite.py -> réponse d'erreur dans <= 35s
echo "  (attente timeout ~30s...)"
START=$(date +%s)
R=$(curl -si "${BASE}/cgi-tests/infinite.py" --max-time 40)
END=$(date +%s)
ELAPSED=$((END - START))

if echo "$R" | grep -qE "HTTP/1.1 (500|502|504)" && [ $ELAPSED -le 35 ]; then
    pass "T20 infinite.py -> timeout + erreur HTTP (${ELAPSED}s)"
elif echo "$R" | grep -qE "HTTP/1.1 (500|502|504)"; then
    fail "T20 infinite.py -> timeout trop long (${ELAPSED}s > 35s)"
else
    fail "T20 infinite.py -> réponse inattendue: $(echo "$R" | head -1)"
fi

# T21 — Serveur toujours vivant après timeout
R=$(curl -si "${BASE}/" --max-time 5)
check_status "T21 Serveur vivant après timeout" "$R" "200"

# ============================================================
section "Groupe 8 — Sortie volumineuse"
# ============================================================

# T22 — spam.php -> réponse complète reçue
R=$(curl -si "${BASE}/cgi-tests/spam.php" --max-time 15)
check_status "T22 spam.php status 200" "$R" "200"
# Vérifie qu'on a bien reçu des données volumineuses (>10KB)
SIZE=$(echo "$R" | wc -c)
if [ "$SIZE" -gt 10000 ]; then
    pass "T22 spam.php réponse volumineuse (${SIZE} bytes reçus)"
else
    fail "T22 spam.php réponse trop petite (${SIZE} bytes)"
fi

# ============================================================
# RÉSULTATS
# ============================================================
TOTAL=$((PASS + FAIL))
echo ""
echo "============================================"
if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}Results: ${PASS}/${TOTAL} passed — All tests OK!${NC}"
else
    echo -e "Results: ${GREEN}${PASS}${NC}/${TOTAL} passed, ${RED}${FAIL} failed${NC}"
fi
echo "============================================"

# Cleanup
rm -f /tmp/cgi_sleep_result.txt

exit $FAIL
