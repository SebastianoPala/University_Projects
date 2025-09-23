<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/room_management.php";
    require_once "../utility/player_management.php";
    
    const MAX_FILE_SIZE = 1*1024*1024;
    session_start();
    unset($_SESSION['ROOM_TYPE']);
    unset($_SESSION['stato_partita']);
    unset($_SESSION['MY_ID']);
    unset($_SESSION['ROOM_ID']);
    unset($_SESSION['turn']);
    unset($_SESSION['NEXT_MOVE']);
    unset($_SESSION['CREATOR']);
    unset($_SESSION['is_being_served']);
    if(!isset($_SESSION['username'])) {
        header("Location: /php/pagine/home.php");
        exit();
    }
    $player_info = getInfo($_SESSION['username'],"player");
    if(!$player_info){
        session_destroy();
        header("Location: /php/pagine/home.php");
        exit();
    }
    $_SESSION["soldi"]= $player_info['soldi'];
    $_SESSION['soldi_old'] = $player_info['soldi'];

    if($player_info['stanza']!== null)
        $_SESSION["ROOM_ID"]=$player_info['stanza'];

    if($player_info['stanza']!== null)
        esciStanza($_SESSION['username'],$player_info['stanza']);

    $error_message="";
    if($_SERVER['REQUEST_METHOD']==="POST"){

        // gestione foto profilo
        if(isset($_POST['change_pfp']) && $_POST['change_pfp']=="RIMUOVI FOTO CORRENTE"){ //rimozione foto profilo

            $hash = hash('sha256',$_SESSION['username']);
            $target = $_SERVER['DOCUMENT_ROOT'] . PFP_FOLDER.$hash.".png";
            if(file_exists($target))
                unlink($target);
            else
                $error_message="Non hai una foto profilo";

        }else if(isset($_POST['change_pfp']) && isset($_FILES['pfp'])){  //aggiunta/modifica foto profilo

            $pfp = $_FILES['pfp'];
            $validExt= ["jpg","jpeg","png"];
            $validMime=["image/jpeg","image/png"];
            $ext = strtolower(pathinfo($pfp['name'],PATHINFO_EXTENSION));

            if ($pfp['error'] !== UPLOAD_ERR_OK) {
                $error_message="Errore nel caricamento del file";
            }else if($pfp['size'] > MAX_FILE_SIZE){
                $error_message="File supera la dimensione massima";
            }else if(!in_array($pfp['type'],$validMime) || !in_array($ext,$validExt)){
                $error_message="File di tipo non supportato";
            }else{
                $tmp_pfp = $pfp['tmp_name'];
                $hash = hash('sha256',$_SESSION['username']); //utilizzo un hash del nome utente per memorizzare la foto profilo nel file system
                $dest = $_SERVER['DOCUMENT_ROOT'] . PFP_FOLDER.$hash.".png";
                if(!move_uploaded_file($tmp_pfp, $dest))
                    $error_message="Errore nello spostamento del file";
                
            }

        }else if($_SESSION['soldi']>0){ //gestione creazione e entrata stanze
            $room_id=0;
            $room_already_exists=true;

            if(isset($_POST["room_type"]) && $_POST["room_type"] =="Sin"){ //se la stanza è singleplayer, vado subito alla pagina di gioco
                $_SESSION['ROOM_TYPE']="single";
                $_SESSION['BOT_NUM'] = $_POST['num_players'];
                $_SESSION['ROOM_ID'] ="-";
                header("Location: /php/pagine/gameroom.php");

            }else if(isset($_POST["CREA"]) && isset($_POST["room_type"]) && isset($_POST["num_players"]) ){
                unset($_POST["CREA"]);
                $room_id = creaStanza($_POST["room_type"],$_POST["num_players"]); //aggiungo la stanza al database
                if($room_id === 0){
                    $error_message="Errore nella creazione della stanza";
                }
                $room_already_exists=false;
            }else if(isset($_POST["ROOM_ID"])){ 
                $room_id = $_POST["ROOM_ID"]; //se la stanza esisteva gia, ricevo l'id dall'input dell'utente
            }

            if($room_id){ //gestione ingresso stanze
                if(preg_match("/^[A-Z0-9]{6}$/",$room_id)){
                    $errorjoin=entraStanza($room_id,$_SESSION["username"],$room_already_exists); //aggiungo l'utente alla stanza
                    if($errorjoin===""){
                        $_SESSION['ROOM_TYPE']="multi";
                        header("Location: /php/pagine/gameroom.php");
                    }else{
                        $error_message="Errore: ".$errorjoin;
                    }
                }else{
                    $error_message="Errore: Id nel formato sbagliato";
                }
            }else
                $error_message = "Errore nella creazione della stanza";
        }else
            $error_message="Errore: non puoi giocare senza soldi";
    }

?>

<!DOCTYPE html>
<html lang = "it">
    <head>
        <title>Main Menu</title>
        <meta charset = "utf-8">
        <meta name = "author" content = "Sebastiano Pala">
        <link rel="stylesheet" href="/CSS/Layout.css">
        <link rel="stylesheet" href="/CSS/MainMenu.css">
        <script>
            window.appData = {
                username: "<?= htmlspecialchars($_SESSION['username']) ?>",
                badAttempt: "<?= htmlspecialchars($error_message) ?>"
            };
        </script>
        <script src="/script/MainMenu.js"></script>
        <script src="/script/layout.js"></script>
    
    </head>
    <body>
        <div id="main_menu" class="page_style container">
            <div id="title">
                <img alt="clubs">
                <img alt="hearts">
                <h1 style="color: white;">POKER NIGHT</h1>
                <img alt="diamonds">
                <img alt="spades">
            </div>
            <div id="player_profile">
                <div id="pfp_container">
                    <img id="edit" src="/Assets/layout/edit.png" alt="edit" draggable="false">
                    <img id="pfp" src="/Assets/profilepictures/<?php 
                        $hash = hash('sha256',$_SESSION['username']);
                        if(file_exists( $_SERVER['DOCUMENT_ROOT'] .PFP_FOLDER.$hash.".png"))
                            echo $hash;
                        else
                            echo "default";
                            ?>.png" alt="pfp">
                </div>
                <div id="player_info">
                    <?php echo "<p id='username'>User: {$_SESSION['username']}</p><p id='soldi'>Soldi: {$_SESSION['soldi']}</p>"?>
                </div>
            </div>
            <nav id="menu">
                <button id="lista_stanze" class="page_style">
                    <img alt="diamonds">
                    STANZE
                    <img alt="clubs">
                </button>
                <button id="crea_stanza" class="page_style">
                    <img alt="diamonds">
                    CREA STANZA
                    <img alt="hearts">
                </button>
                <button id="join_stanza" class="page_style">
                    <img  alt="hearts">
                    UNISCITI
                    <img  alt="spades">
                </button>
                <button id="show_leaderboard" class="page_style">
                    <img  alt="clubs">
                    LEADERBOARD
                    <img  alt="diamonds">
                </button>
                <a href="/php/pagine/logout.php" class="page_style">
                    <img  alt="spades">
                    LOGOUT
                    <img  alt="clubs">
                </a>
                <a href="/html/guida.html" class="page_style">
                    <img  alt="hearts">
                    DOCUMENTAZIONE
                    <img  alt="diamonds">
                </a>
            </nav>
        </div>
        <div id="window" class="page_style container" hidden>

        </div>
    </body>
</html>