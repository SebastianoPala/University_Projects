<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/room_management.php";
    require_once "../utility/player_management.php";
    require_once "../utility/game_management.php";


    session_start();
    if(!isset($_SESSION['username'])) {
        header("Location: /php/pagine/home.php");
        exit();
    }
    $full_info = getInfo($_SESSION['username'],"all");
    if(!$full_info){
        header("Location: /php/pagine/home.php");
        exit();
    }
    if($_SESSION['ROOM_TYPE']=="multi"){
        if($full_info['soldi']<=0){
            header("Location: /php/pagine/main_menu.php");
            exit();
        }
        if($full_info['stanza']!== null)
        $_SESSION["ROOM_ID"]=$full_info['stanza'];
        else{
            header("Location: /php/pagine/main_menu.php");
            exit();
        }
        if($full_info['ID'] === null){ 
            echo "La stanza è stata chiusa. Reindirizzamento al menu principale...";
            header("refresh:4; url= /php/pagine/main_menu.php");
            exit();
        }
        if($full_info['turn'] === null){ //la stanza non è ancora stata inizializzata
            //segnalo che la stanza è inizializzata e gli utenti possono entrare
            if(setStateAndTurn($full_info['ID'])<0){
                echo "Errore nell'inizializzazione della stanza. Reindirizzamento al menu principale...";
                header("refresh:5; url= /php/pagine/main_menu.php");
                exit();
            }
            $_SESSION['CREATOR'] = true;
        }
    }else
        $_SESSION['CREATOR'] = true;
    unset($_SESSION['stato_partita']);
    $_SESSION["soldi"] = $full_info['soldi']; // la variazione verra salvata al di fuori del database
    $_SESSION["soldi_old"] = $full_info['soldi'];
    
?>

<!DOCTYPE html>
<html lang = "it">
    <head>
        <meta name = "author" content = "Sebastiano Pala">
        <title>GameRoom</title>
        <meta charset = "utf-8">
        <link rel="stylesheet" href="../../CSS/Layout.css">
        <link rel="stylesheet" href="../../CSS/GameRoom.css">
        <script>
            window.appData = {
                isCreator: <?= isset($_SESSION['CREATOR']) && $_SESSION['CREATOR'] ? 'true' : 'false' ?>,
                username: "<?= htmlspecialchars($_SESSION['username']) ?>",
            };
        </script>
        <script src="/script/GameRoom.js"></script>
    </head>
    <body>

            <div id="tavolo">
                <button id="fast">SLOW</button>
                <h2 id="timer"></h2>
                <?php if(isset($_SESSION['CREATOR']) && $_SESSION['CREATOR']==true):?>
                    <button id="start_button" class="darken">AVVIA</button>
                    <h1 id="entry_message" hidden>LA STANZA E' VUOTA</h1>
                <?php else: ?>
                    <h1 id="entry_message">IN ATTESA CHE L'HOST<br>AVVII LA PARTITA...</h1>
                <?php endif; ?>
                <?php if(isset($_SESSION['ROOM_ID']) && $_SESSION['ROOM_ID']!= "-"):?>
                <div id="room_id">ID: #<?php echo $_SESSION['ROOM_ID']?></div>
                <?php endif; ?>
                <div id="player_top" class="main_container top" hidden>
                    <div class="chip_container">
                        <img src="/Assets/chips/poker-chips.png" class="chipstack" alt="stack">
                    </div>
                    <div class="card_container">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                    </div>
                    <div class="player_profile">
                        <img src ="/Assets/profilepictures/default.png" alt="pfp">
                        <div class="player_info">

                        </div>
                    </div>
                </div>
                <div id="total_bet" hidden>
                    <img src="/Assets/chips/poker-chips.png" alt="stack" class="chipstack">
                    <p id="bet_board" class="message_board"></p>
                    <p id="my_bet_board" class="message_board"></p>
                    <p id="needed_bet_board" class="message_board"></p>
                </div>
                <div id="player_left" class="main_container left" hidden>
                    <div class="player_profile">
                        <img src ="/Assets/profilepictures/default.png" alt="pfp">
                        <div class="player_info"></div>
                    </div>
                    <div class="card_container">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                    </div>
                    <div class="chip_container">
                        <img src="/Assets/chips/poker-chips.png" alt="stack" class="chipstack">
                    </div>
                </div>
                
                <div id="HUD" hidden>
                    <div id="player_bottom" class="main_container" hidden>
                        <div class="player_profile">
                            <img id="pfp" src="/Assets/profilepictures/<?php 
                                $hash = hash('sha256',$_SESSION['username']);
                                if(file_exists( $_SERVER['DOCUMENT_ROOT'] .PFP_FOLDER.$hash.".png"))
                                    echo $hash;
                                else
                                    echo "default";
                            ?>.png" alt="pfp">
                            <div class="player_info">

                            </div>
                        </div>
                        <div class="card_container">
                            <img src="/Assets/cards/backcarta.png" alt="card">
                            <img src="/Assets/cards/backcarta.png" alt="card">
                        </div>
                        <div class="chip_container">
                            <img src="/Assets/chips/poker-chips.png" alt="stack" class="chipstack">
                        </div>
                    </div>
                    <div id="moves">
                        <button id="check" class="darken" disabled>CHECK</button>
                        <div id="bet-row">
                            <button id="bet" class="bet darken" disabled>BET</button>
                            <input id="betamount" type="number" min="1" placeholder="Amount">
                        </div>
                        <button id="fold" class="darken" disabled>FOLD</button>
                    </div>
                </div>

                <div id="player_right" class="main_container right" hidden>
                    <div class="player_profile">
                        <div class="player_info">

                        </div>
                        <img src ="/Assets/profilepictures/default.png" alt="pfp">
                    </div>
                    <div class="card_container">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                        <img src="/Assets/cards/backcarta.png" alt="card">
                    </div>
                    <div class="chip_container">
                        <img src="/Assets/chips/poker-chips.png" alt="stack" class="chipstack">
                    </div>
                </div>
                
                <div id="dealer" class="main_container">
                    <div class="card_container">

                    </div>
                </div>
            </div>
            <a id="exit" class="darken" href="/php/pagine/main_menu.php">EXIT</a>

    </body>
</html>