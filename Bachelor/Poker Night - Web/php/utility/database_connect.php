<?php
    function db_connect(){    
        $connection = mysqli_connect(DBHOST, DBUSER, DBPASS, DBNAME);
        if(mysqli_connect_error() !== null){
            die("Errore di connessione " . mysqli_connect_error());
        }
        return $connection;
    }
?>