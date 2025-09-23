<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/room_management.php";
    require_once "../utility/player_management.php";
    require_once "../utility/game_management.php";
    require_once "../utility/singleplayer_management.php";
    //file che gestisce la logica della stanza e restituisce lo stato al chiamante
    
    const MAX_AWAIT_TIME = 55;
    session_start();
    $is_error=false;
    $response['successo'] = true;
    if($_SERVER['REQUEST_METHOD']==="GET" && isset($_SESSION['ROOM_ID'])){
        if(isset($_SESSION['is_being_served']) && $_SESSION['is_being_served'] === true){
            setErrorResponse("DOUBLE REQUEST FROM SAME USER");
            echo json_encode($response);
            exit;
        }else
        $_SESSION['is_being_served'] = true;
        header('Content-Type: application/json');
        
        if(isset($_SESSION['ROOM_TYPE']) && $_SESSION['ROOM_TYPE']=="single"){ //sezione di risposta giocatore singolo
            unset($_SESSION['CREATOR']);
            $response = playSinglePlayerTurn();
            $response['successo'] = true;
            $_SESSION['is_being_served'] = false;
            echo json_encode($response);
            exit;
        }
        
        $full_info=getInfo($_SESSION['username'],"all");
        if(!$full_info){
            setErrorResponse("Richiesta invalida");
            $_SESSION['is_being_served'] = false;
            echo json_encode($response);
            exit;
        }

        if(!$full_info['stanza'] && $full_info['soldi']==0 && isset($_SESSION['ROOM_ID'])){ //se ho finito i soldi posso comunque ricevere le mosse
            $room_info = getInfo($_SESSION['ROOM_ID'],"room");                              // nel caso venga rimosso da una stanza prima di aver
            if($room_info)                                                                  ///ricevuto la mossa di fine partita
            $response['moves']=readMoves($room_info['storia_mosse']);
            else
            $response['moves']=[];
            $_SESSION['is_being_served'] = false;
            echo json_encode($response);
            exit;
        }

        $_SESSION['soldi']=$full_info['soldi']; // tengo i soldi aggiornati in caso di vincita
        $_SESSION['soldi_old']=$full_info['soldi'];
        
        // SEZIONE INIZIALIZZAZIONE STANZA
        if(isset($_SESSION['CREATOR']) && $_SESSION['CREATOR']===true){ //primo messaggio del creatore. verifico se posso iniziare la partita
            if($full_info['giocatori']<=1){
                setErrorResponse("STANZA VUOTA");
            }else{
                $players = getOtherPlayers($full_info['stanza']);
                if(!empty($players)){
                    $room = new Room($full_info);
                    $_SESSION['MY_ID']= $room->addPlayer($_SESSION); //dovrei ottenere sempre 0
                    foreach($players as $player)
                        $room->addPlayer($player);
                    
                    $room->setEverythingUp();

                    if(($new_turn=setStateAndTurn($full_info['stanza'],$_SESSION['MY_ID'],$room))<0) // il creatore è sempre il giocatore 0 in teoria
                        setErrorResponse("IMPOSSIBILE AVVIARE LA PARTITA");
                }else
                    setErrorResponse("STANZA VUOTA");
            }
            if($response['successo'] === false){
                echo json_encode($response);
                $_SESSION['is_being_served'] = false;
                exit;
            }else{
                unset($_GET['move']);
                $_GET['info']="all";
            }
        }
        unset($_SESSION['CREATOR']);



        $is_hijacked = false;
        $bad_move = true;

        if(!$is_error && isset($_GET['info']) && !isset($_GET['move'])){
            
            //SEZIONE INVIO INFORMAZIONI AL CLIENT
            $bad_move = false;

            if($full_info['stato_partita']==="-"){
                setErrorResponse("hold"); // errore "hold" indica che la stanza non è ancora stata inizializzata                
            }else{

                $turn_timestamp = new DateTime($full_info['turn_timestamp']);
                $now = new DateTime();
                $time_passed = $now->getTimestamp() - $turn_timestamp->getTimestamp();
                
                if($_GET['info']==="all"){ //con all si ottiene la lista dei giocatori e le proprie carte

                    $state=unserialize($full_info['stato_partita']);
                    
                    $list=[]; //conterrà la lista dei giocatori
                    $pfps=[]; //conterrà i percorsi delle foto profilo
                    $who_folded = []; //conterrà chi ha foldato (utile per chi esce e rientra o fa da spettatore)
                    if(count($state['p'])<=1){ //se ho un solo giocatore interrompo e chiudo
                        $response['over']=true;
                    }else{
                        $my_bet = 0;
                        foreach($state['p'] as $serialized_player){ //deserializzo solo i giocatori, il resto non mi serve
                            $player = new Player();
                            $player->unserialize($serialized_player['p']);

                            if($player->status ==="fold")
                                $who_folded[]=[$serialized_player['k'],["fold"]]; //mi segno chi ha foldato

                            $list[$serialized_player['k']] = [
                                'u'=>$player->username,
                                's'=>$player->soldi];

                            if($_SESSION['username']===$player->username){ //se incontro il mio username (non è scontato) mi salvo le informazioni per giocare
                                $_SESSION['MY_ID']=$serialized_player['k']; // o riprendere la partita
                                $response['cards']=$player->cards;
                                $my_bet= $player->bet;
                                $pfps[$serialized_player['k']]="-"; //la mia foto profilo ce l'ho gia
                            }else{
                                $hash = hash('sha256',$player->username);
                                $target = $_SERVER['DOCUMENT_ROOT'] . PFP_FOLDER.$hash.".png";
                                $pfps[$serialized_player['k']]= file_exists($target) ? $hash : "-"; //ottengo la foto profilo degli altri
                            }
                        }
                        readMoves($full_info['storia_mosse']); //faccio una lettura a vuoto per non ricevere le mosse vecchie, non mi servono
                        
                        $response['bet']= $state['t'];
                        $response['highest']=$state['b'];
                        $response['my_bet'] = $my_bet;
                        $response['deal'] = $state['h']; //carte del dealer
                        $response['list']= $list;
                        $response['pfps']= $pfps;
                        $response['time']= $time_passed; //quanto tempo è passato dall'ultimo cambio di turno
                    }
                }
                $response['moves']= $_GET['info'] !="all" ? readMoves($full_info['storia_mosse']) : $who_folded; //se ho richiesto all, invio solo chi ha foldato, altrimenti le mosse
                $response['turn']=$full_info['turn'];
                
                if($time_passed>MAX_AWAIT_TIME && count($response['moves'])==0){
                    $is_hijacked=true;
                    hijackSession($full_info);
                }
            } 

        }


        if(!$is_error &&  isset($_GET['move']) && !isset($_GET['info']) && isset($_SESSION['MY_ID']) && $full_info['turn'] === $_SESSION['MY_ID']){
            
            $bad_move = false;
            
            if($full_info['stato_partita']==="-"){
                setErrorResponse("hold");
            }else{
                
                //SEZIONE DI GESTIONE MOSSE DEI GIOCATORI

                $room = new Room($full_info); //carico la stanza
                $player = $room->player_list[$_SESSION['MY_ID']];
                $player->soldi = $_SESSION['soldi'];

                //eseguo la mossa ricevuta, verificando sia corretta
                $move = checkMoveOutcome($_GET['move'],$_GET['amount'] ?? 0,$room,$player);
                if(!$move){
                    setErrorResponse("BAD MOVE");
                    echo json_encode($response);
                    exit;
                }
                $_SESSION['soldi'] = $player->soldi;
                
                //salvo la mossa nella storia
                $moves_history = new Storia($full_info['storia_mosse']);
                $moves_history->setNewMove($_SESSION['MY_ID'],$move);
                
                $status_outcome = checkGameStatus($room,$moves_history); //verifico come è cambiato lo stato dopo la mia mossa

                $response['over']=false; //response['over'] indica se la PARTITA si è conclusa
                $alone = false; //alone è true quando si è rimasti da soli nella stanza


                //SEZIONE FINE MANO
                if($status_outcome['over']){ // status_outcome['over'] indica se LA MANO è giunta al termine
                    $winner = $room->checkWinner(); //guardo chi è il vincitore e perche ha vinto
                    $all_cards=$room->getAllCards(); //ottengo le carte da inviare ai giocatori

                    $moves_history->setNewMove($winner['id'],['win',$winner['comb'],$all_cards]); //salvo la vincita

                    if($winner['id'] !== $_SESSION['MY_ID']) //chiamo updateWinnerMoney SOLO se non sono io il vincitore
                        updateWinnerMoney($room->player_list[$winner['id']],$room->total_bet);
                    else
                        $_SESSION['soldi'] += $room->total_bet; //i soldi verranno aggiornati con setStateAndTurn()
                    
                    if($full_info['giocatori']<=1){ //se sono usciti tutti, mi preparo a uscire
                        $response['over'] = true; 
                        $alone =true;
                    }else{
                        $players = getOtherPlayers($full_info['stanza']); //prendo i giocatori dentro la stanza con soldi >0 e prendo il piatto
                        if(!empty($players)){ 
                            $room = new Room();
                            if($_SESSION['soldi']>0 && !$is_hijacked){ //mi aggiungo alla stanza solo se ho ancora soldi e nessuno ha fatto il turno al posto mio
                                $_SESSION['MY_ID']= $room->addPlayer($_SESSION);
                            }
                            foreach($players as $player){
                                $room->addPlayer($player);
                            }
                            if($room->first_unassigned_id>1){ //se ho aggiunto piu di un giocatore (me compreso),inizializzo la stanza
                                $room->setEverythingUp();
                            }else{
                                $response['over']=true;
                            }
                        }else{
                            $response['over']=true;
                        }
                    }
                }


                //SEZIONE DI SALVATAGGIO STATO

                //salvo lo stato, le mosse, setto il turno e aggiorno i soldi
                $response['turn']=setStateAndTurn($full_info['stanza'],$_SESSION['MY_ID'],$room,$moves_history);
                //ottengo le mosse da inviare al client
                $response['moves']=$moves_history->getLastMoves();

                //se sono da solo, esco dalla stanza
                if($is_hijacked){
                    esciStanza($_SESSION['username'],$_SESSION['ROOM_ID']);
                    restoreSession(); // ritorno allo stato originale
                }

                if($alone || ($_SESSION['soldi'] == 0 && $response['over']==true)){
                    esciStanza($_SESSION['username'],$_SESSION['ROOM_ID']);
                    unset($_SESSION['ROOM_ID']);
                }
                
            }
        }if($bad_move)
            setErrorResponse("ERROR: BAD MOVE");
            
        echo json_encode($response);
        $_SESSION['is_being_served'] = false;
    }   

    function setErrorResponse($errormess){
        header('Content-Type: application/json');
        global $is_error;
        global $response;
        $is_error=true;
        $response['successo']=false;
        $response['error']=$errormess;
    }
?>
