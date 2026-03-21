<?php
error_log("infinite.php: Infinite loop is running... Webserv kill process after 30sec + send 502");

while (true) {
    sleep(1);
}
