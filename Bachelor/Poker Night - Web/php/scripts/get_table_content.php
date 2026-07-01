<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";

    if($_SERVER["REQUEST_METHOD"]=="GET"){
        $connection = db_connect();
        $num_rows = 50;
        $request_type = $_GET['type'] ?? null;

        if($request_type==="LEADERBOARD" || $request_type==="STANZE"){

            if($request_type === "LEADERBOARD") //eseguo 2 query differenti a seconda del tipo di richiesta
                $query = mysqli_prepare($connection,"SELECT username,soldi FROM utenti u ORDER BY soldi DESC,username ASC LIMIT ?");
            else
                $query = mysqli_prepare($connection,"SELECT ID,giocatori,`max giocatori` FROM stanze s WHERE pubblica = TRUE AND giocatori < `max giocatori` AND giocatori > 0 ORDER BY giocatori DESC LIMIT ?");
            //nel caso delle stanze, restituisco solo le stanze pubbliche inizializzate NON piene
            mysqli_stmt_bind_param($query,"i",$num_rows);
            if(!mysqli_stmt_execute($query)){
                mysqli_stmt_close($query);
                http_response_code(500); 
                echo json_encode(['successo' => false, 'dati' => 'Errore nel database']);
                exit;
            }
            $rows = mysqli_stmt_get_result($query);
            $result = mysqli_fetch_all($rows,MYSQLI_ASSOC); // necessario per tenere il nome delle colonne come chiavi
            
            mysqli_stmt_close($query);
            mysqli_close($connection);
            
            header('Content-Type: application/json');
            echo json_encode([
                'successo' => true,
                'dati' => $result
            ]);
        }
    }
?>