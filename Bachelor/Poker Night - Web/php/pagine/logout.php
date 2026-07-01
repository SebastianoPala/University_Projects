<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/room_management.php";
    require_once "../utility/player_management.php";
    
    session_start();
    if(isset($_SESSION["ROOM_ID"]) && isset($_SESSION['username']))
        esciStanza($_SESSION['username'],$_SESSION["ROOM_ID"]);

    session_destroy();
    header("Location: /php/pagine/home.php");
    exit();
?>