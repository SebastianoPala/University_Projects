<?php
    require_once "../utility/game_management.php";

    const FOLD_CHANCE = 15;
    const BET_CHANCE = 35;
    
    $bot_names = ["Gwyn","Pursuer","Midir"]; //nomi statici dei bot
    $extra_bot = "Gael"; //nome extra, nel caso ci sia un giocatore omonimo

    $player_folded=true; //indica se il giocatore non è piu in grado di giocare (fold/all-in)

    function playSinglePlayerTurn(){
        global $player_folded;

        $room = loadRoom(); //carico lo stato della stanza
        $my_id = $_SESSION['MY_ID'];

        $alone = false; //vera se tutti i bot hanno finito i soldi
        
        $_SESSION['turn']=$my_id;
        if(isset($_GET['info']) && $_GET['info']==="all"){ //invio le informazioni necessarie alla creazione della stanza
            $list=[];
            $pfps=[];
            foreach($room->player_list as $id => $pl){
                $list[$id] = [
                    'u'=>$pl->username,
                    's'=>$pl->soldi]; 
                if($_SESSION['username']===$pl->username)
                    continue;
                
                $target = $_SERVER['DOCUMENT_ROOT'] . PFP_FOLDER.$pl->username.".png"; //le foto profilo dei bot hanno i nomi in chiaro
                $pfps[$id]= file_exists($target) ? $pl->username : "-";
            }
            $pfps[$my_id]="-";
            
            //invio le informazioni al giocatore per costruire la stanza
            $response['cards']=$room->player_list[$my_id]->cards;
            $response['bet']=$room->total_bet;
            $response['highest']= 0;
            $response['my_bet'] = 0;
            $response['deal'] = $room->house_cards;
            $response['list']= $list;
            $response['pfps']= $pfps;
            $response['moves']= [];

        }else if(isset($_GET['move'])){ //eseguo la mossa del giocatore e simulo le mosse degli altri giocatori

            $moves_history = []; //contenitore delle mosse
            $myself = $room->player_list[$my_id];
            $move = checkMoveOutcome($_GET['move'],$_GET['amount'] ?? 0,$room,$myself); //eseguo la mossa
            if(!$move){ //e verifico fosse valida
                $response['successo']=false;
                $response['error']= "BAD MOVE";
                return;
            }
            $moves_history[]=[$my_id,$move];
            $_SESSION['soldi'] = $myself->soldi; //aggiorno i soldi in quanto possono essere stati modificati dalla mossa precedente

            $status_outcome = checkGameStatus($room); //verifico lo stato della stanza, ovvero se sono rimasti giocatori che devono ancora giocare

            if(isset($status_outcome['deal']) && count($status_outcome['deal'])){ //aggiungo tutte le carte estratte dal dealer al contenitore delle mosse
                foreach($status_outcome['deal'] as $card){
                    array_push($moves_history,['d',$card]);
                }
            }

            $is_over=false; //variabile che indica la fine di una mano
            
            $test=0; //semplice variabile con lo scopo di scongiurare cicli infiniti
            while($player_folded && !$is_over && $test<20){
                $test++;

                if($_GET['move']=="fold" || $_SESSION['soldi']==0)
                    $player_folded=true;
                else
                    $player_folded = false;
                
                if($status_outcome['over']==false){ //se la partita non si è conclusa, eseguo le mosse dei bot
                    $moves_over = simulateBotMoves($room);
                    array_push($moves_history,...$moves_over['moves']); //e le inserisco nel contenitore
                    $is_over = $moves_over['over'];
                }else
                    $is_over = $status_outcome['over'];
      
                $response['over']=false; //variabile che indica la fine della partita, vera se soldi terminati oppure unico giocatore rimasto

                if($is_over){

                    $player_folded=false; //resetto player_folded allo stato iniziale
                    $alone = true; //inizializzo alone, che verrà modificata a breve

                    $winner = $room->checkWinner(); //controllo il vincitore e la combinazione con cui ha vinto
                    $all_cards=$room->getAllCards(); //preparo le carte scoperte da inviare

                    $moves_history[] = [$winner['id'],['win',$winner['comb'],$all_cards]]; //inserisco la vincita
                    $room->player_list[$winner['id']]->soldi += $room->total_bet; //e assegno il premio
                    
                    foreach($room->player_list as $id => $pl){
                        if($id === $my_id)
                        continue;
                        if($pl->soldi>0){
                            $alone=false; //se trovo almeno un altro bot ingame, il gioco prosegue
                            break;
                        }
                        unset($room->player_list[$id]); //rimuovo i perdenti
                    }
                    if(!$alone && $room->player_list[$my_id]->soldi>0){
                        
                        $new_pl = array_values($room->player_list); //compatto il vettore dei giocatori

                        $room = new Room(); //reinizializzo la stanza
                        foreach($new_pl as $player)
                            $room->addPlayer(['username'=>$player->username,'soldi'=>$player->soldi]);
                        $room->giveCards();
                        $hcard = $room->dealHouseCard();
                        $room->getPot();
                    }
                    break;
                }
            }
            
            $response['moves'] = $moves_history; //invio il contenitore con la storia delle mosse
        }
        
        $_SESSION['soldi'] = $room->player_list[$my_id]->soldi;
        if($_SESSION['soldi'] == 0 || $alone){ //se ho finito i soldi oppure sono rimasto solo concludo la partita
            unset($_SESSION['stato_partita']); //distruggo lo stato della stanza
            $response['over']=true;
        }else
            saveRoom($room);

        $response['turn'] = $my_id; //il turno sarà sempre quello del giocatore

        return $response;
    }

    function simulateBotMoves(Room $room){

        $next = $room->findNextTurn($_SESSION['turn']);
        $moves = [];
        $status_outcome['over']=false;

        for($i=0;$i<$room->first_unassigned_id;$i++){

            $pl = $room->player_list[$next];
            if($next != $_SESSION['turn'] && $next != $_SESSION['MY_ID']){
                $rand_move = playRandomMove($room,$pl); //genero una mossa e la eseguo
                if($rand_move)
                    $moves[] = [$next,$rand_move];

                $status_outcome = checkGameStatus($room); //poi verifico l'effetto sulla partita
                if(isset($status_outcome['deal']) && count($status_outcome['deal'])){
                    foreach($status_outcome['deal'] as $card){
                        array_push($moves,['d',$card]);
                    }
                }
                if($status_outcome['over']==true)
                    break;
            }else
                break;
            $_SESSION['turn']=$next;
            $next=$room->findNextTurn($next);
        }
        return ['moves'=>$moves,'over'=>$status_outcome['over']]; //restituisco le mosse e la variabile che indica che la partita è terminata
    }

    function playRandomMove(Room $room,Player $player){

        global $player_folded;
        $move = null;
        $num = rand(1,100);
        //genero un numero da 1 a 100, e la mossa dipenderà dall'intervallo del numero
        if($player->status == "wait" || $player->status == "raise"){
            $bet = 0;
            if(!$player_folded){
                if($num <FOLD_CHANCE){
                    $move = "fold";
                }else if($num > 100 - BET_CHANCE && $room->highest_bet < $player->soldi){
                    $move="bet";
                    $bet = $room->highest_bet - $player->bet; //bet parte sempre dalla puntata piu alta della stanza
                    $extra = floor($player->soldi*0.05);
                    if($extra>0){
                        $bet += $extra; 
                    }else
                        $move = "check"; //bet deve essere strettamente maggiore della puntata precedente, se non lo è faccio call
                }else
                    $move = "check";
            }else
                $move = "check";
            $move = checkMoveOutcome($move,$bet,$room,$player); //eseguo ufficialmente la mossa
        }
        return $move;
    }

    function saveRoom(Room $room){
        $_SESSION['stato_partita']=$room->serialize();
    }

    function loadRoom(){
        global $bot_names;
        global $extra_bot;
        if(isset($_SESSION['stato_partita'])){ //se lo stato esiste lo ricarico
            $room = new Room($_SESSION);
        }else{                              //altrimenti creo una stanza nuova
            $room = new Room();
            $_SESSION['MY_ID'] = $room->addPlayer($_SESSION);
            $pos = array_search($_SESSION['username'],$bot_names);
            if($pos!==false) //se il giocatore è un ononimo dei bot, sostituisco
                $bot_names[$pos]=$extra_bot;
                
            for($i=0;$i<$_SESSION['BOT_NUM']-1;$i++){ //i bot hanno i soldi proporzionali ai soldi del giocatore
                $info = ['username'=>$bot_names[$i],'soldi'=>$_SESSION['soldi']+floor(0.1*($i+1)*$_SESSION['soldi'])];
                $room->addPlayer($info);
            }
            $room->giveCards();
            $room->dealHouseCard();
            $room->getPot();
        }
        return $room;
    }

?>