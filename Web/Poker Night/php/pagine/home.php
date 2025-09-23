<?php

session_start();
if(isset($_SESSION['username'])) {
    header("Location: /php/pagine/main_menu.php");
    exit();
}
?>
<!DOCTYPE html>
<html lang = "it">
    <head>
        <meta charset = "utf-8">
        <meta name = "author" content = "Sebastiano Pala">
        <title>
            Index
        </title>
        <link rel="stylesheet" href="/CSS/Layout.css">
        <script src="/script/layout.js"></script>
    </head>
    <body>
        <div class="page_style container">
            <div id="title">
                <img  alt="clubs">
                <img  alt="hearts">
                <h1 style="color: white;">POKER NIGHT</h1>
                <img  alt="diamonds">
                <img  alt="spades">
            </div>
            <h2 style="color: white;">Benvenuto, effettua l'accesso per iniziare a giocare</h2>
            <nav id="menu">
                <a href="/php/pagine/login.php" class="page_style">
                    <img  alt="clubs">
                    LOGIN / SIGNUP
                    <img  alt="diamonds">
                </a>
                <a href="/html/guida.html" class="page_style">
                    <img  alt="hearts">
                    DOCUMENTAZIONE
                    <img  alt="spades">
                </a>
            </nav>
        </div>
    </body>
</html>