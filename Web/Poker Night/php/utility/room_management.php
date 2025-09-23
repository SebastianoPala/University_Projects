<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/player_management.php";

    function generaIdStanza(){ //genera un id casuale per le stanze
        $alfabeto = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        $lunghezza = strlen($alfabeto);

        $result = "";

        $random = random_bytes(6);
        for($i=0;$i<6;$i++){
            $result.= $alfabeto[ord($random[$i]) % $lunghezza];
        }
        return $result;
    }

    function creaStanza($room_type_string,$max_giocatori){
        if($max_giocatori>4 || $max_giocatori<2)
            return 0;
        if($room_type_string !== "Pub" && $room_type_string!== "Pri")
            return 0;
        $room_type = $room_type_string ==="Pub" ? 1:0;
        $connection = db_connect();

        $query = mysqli_prepare($connection,"SELECT COUNT(*) as n FROM stanze s WHERE s.ID  = ?");
        $room_id=0;
        $idfound=false;
        for($i = 0; $i<10; $i++){
            $room_id = generaIdStanza(); //creo un id, e verifico MAX 10 volte sia univoco per sicurezza
            mysqli_stmt_bind_param($query,"s",$room_id);
            if(!mysqli_stmt_execute($query)){
                break;
            }
            $result = mysqli_stmt_get_result($query);
            $row = mysqli_fetch_assoc($result);
            if($row['n']=== 0){
                $idfound=true;
                break;
            }
        }
        if(!$idfound){
            mysqli_stmt_close($query); 
            mysqli_close($connection);
            return 0;
        }
        $query = mysqli_prepare($connection,"INSERT INTO stanze (ID,pubblica,`max giocatori`) VALUES(?,?,?)"); //creo l'entrata vera e proria nel database
        mysqli_stmt_bind_param($query,"sii",$room_id,$room_type,$max_giocatori);
        if(!mysqli_stmt_execute($query) || mysqli_stmt_affected_rows($query)<0){
            mysqli_stmt_close($query); 
            mysqli_close($connection);
            return 0;
        }
        
        mysqli_stmt_close($query);
        mysqli_close($connection);
        return $room_id;
    }

    function entraStanza($room_id,$username,$is_initialized = true){

        $_SESSION['CREATOR'] = !$is_initialized;
        $error="";
        $room = getInfo($room_id,"room");
        
        if($room===false){
            $error = "Errore nella query";
        }else if($room===null){
            $error = "Non esiste una stanza con questo Id";
        }else if($room['max giocatori'] === $room['giocatori']){
            $error = "Stanza piena";
        }else{

            $connection = db_connect(); //"prenoto" il posto incrementando "giocatori"
            $query = mysqli_prepare($connection,"UPDATE stanze SET giocatori = giocatori + 1 WHERE ID=? AND giocatori < `max giocatori` AND (turn IS NOT NULL) = ?"); // entro solo in stanze inizializzate a meno che io non sia il creatore
            mysqli_stmt_bind_param($query,"si",$room_id,$is_initialized);
            if(!mysqli_stmt_execute($query) || mysqli_stmt_affected_rows($query)<1){
                $error ="Impossibile entrare nella stanza";
            }else{
                if(!addRoomToPlayer($connection,$username,$room_id)){ // una volta "prenotato" il posto all'interno della stanza incrementando "giocatori",associo la stanza al giocatore
                    $error="Impossibile aggiungere la stanza al giocatore";
                }else
                    $_SESSION["ROOM_ID"] = $room_id;
            }
            mysqli_stmt_close($query);
            mysqli_close($connection);
        }
        return $error;
    }

    function esciStanza($username,$room){
        $return = true;
        $connection = db_connect();
        $query = mysqli_prepare($connection,"UPDATE utenti SET stanza= NULL WHERE username = ? AND stanza IS NOT NULL"); //rimuovo la stanza all utente
        mysqli_stmt_bind_param($query,"s",$username);
        mysqli_stmt_execute($query);

        if(mysqli_stmt_affected_rows($query)<=0){ //l'utente non era in nessuna stanza, evito di decrementare il numero di giocatori
            return false;
        }
        
        $query = mysqli_prepare($connection,"UPDATE stanze SET giocatori = giocatori - 1 WHERE ID =? AND giocatori>1"); //riduco il numero di giocatori
        mysqli_stmt_bind_param($query,"s",$room);
        mysqli_stmt_execute($query);

        if(mysqli_stmt_affected_rows($query)<=0){ //se ho modificato meno di una riga, allora la stanza aveva 1 solo giocatore, dunque essendo vuota la distruggo
            $query = mysqli_prepare($connection,"DELETE FROM stanze WHERE ID=? AND giocatori <= 1");
            mysqli_stmt_bind_param($query,"s",$room);
            mysqli_stmt_execute($query);
        }
        mysqli_stmt_close($query);
        mysqli_close($connection);
        return $return;
    }

?>