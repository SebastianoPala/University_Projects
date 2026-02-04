<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    const PFP_FOLDER ="/Assets/profilepictures/";

    function getInfo($target,$mode){ //permette di ottenere informazioni diverse dal database a seconda della modalita specificata
        $connection = db_connect();
        if($mode === "player")
            $query = mysqli_prepare($connection,"SELECT * FROM utenti u WHERE u.username = ?");
        else if($mode === "room")
            $query = mysqli_prepare($connection,"SELECT * FROM stanze WHERE ID = ?");
        else if($mode === "all")
            $query = mysqli_prepare($connection,"SELECT * FROM utenti u LEFT OUTER JOIN stanze s on s.ID = u.stanza WHERE u.username = ?");
        else{
            mysqli_close($connection);
            return false;
        }
        mysqli_stmt_bind_param($query,"s",$target);
        if(!mysqli_stmt_execute($query)){
            mysqli_stmt_close($query); 
            mysqli_close($connection);
            return false;
        }
        $result = mysqli_stmt_get_result($query);
        $info = mysqli_fetch_assoc($result);

        mysqli_stmt_close($query); 
        mysqli_close($connection);
        return $info;
    }

    //funzione chiamata dal creatore della stanza o da chi deve reinizializzarla. Prende tutti i giocatori all'infuori del chiamante,
    //esclude quelli con soldi = 0 (rimuovendoli dalla stanza) e estrae il piatto (5% dei soldi totali) ai rimanenti
    function getOtherPlayers($room_id){
        $connection = db_connect();
        $query = mysqli_prepare($connection,"SELECT username,soldi FROM utenti WHERE stanza = ? AND username <> ?");
        mysqli_stmt_bind_param($query,"ss",$room_id,$_SESSION['username']);
        if(!mysqli_stmt_execute($query)){
            mysqli_stmt_close($query); 
            mysqli_close($connection);
            return false;
        }
        $result = mysqli_stmt_get_result($query);
        $info = [];
        while ($row = mysqli_fetch_assoc($result)) {
            if($row['soldi'] === 0){ //se ha finito i soldi lo rimuove
                esciStanza($row['username'],$_SESSION['ROOM_ID']);
            }else
                $info[] = $row;
        }
        //prendo il piatto
        if($_SESSION['soldi']>0){
            $im_in = 1;
            $im_in_cond = "";
        }else{
            $im_in=0;
            $im_in_cond = "AND username <> ".$_SESSION['username'];
        }
        if( (count($info) + $im_in) > 1 ){ //verifico se ci sono almeno 2 giocatori
            mysqli_autocommit($connection,false);
            $query = mysqli_prepare($connection,"UPDATE utenti SET soldi = soldi - floor(soldi*0.05) WHERE stanza = ? and soldi>0 ".$im_in_cond);
            mysqli_stmt_bind_param($query,"s",$room_id);
            if(!mysqli_stmt_execute($query) || mysqli_stmt_affected_rows($query)<=1){
                mysqli_rollback($connection); //se la query non è andata a buon fine, è possibile che fosse rimasto un solo giocatore. Restituisco i soldi
                $info=[];
            }else
                mysqli_commit($connection);
        }
        mysqli_stmt_close($query); 
        mysqli_close($connection);
        return $info;
    }
    
    function addRoomToPlayer($connection,$username,$room_id){ //aggiungo la stanza nella tabella degli utenti
        $result = true;
        $query = mysqli_prepare($connection,"UPDATE utenti SET stanza = ? WHERE username = ? AND stanza IS NULL");
        mysqli_stmt_bind_param($query,"ss",$room_id,$username);
        if(!mysqli_stmt_execute($query) || mysqli_stmt_affected_rows($query)<1){
            $result= false;
        }
        mysqli_stmt_close($query); 
        return $result;

    }

?>