<?php
    require_once "../utility/database_parameters.php";
    require_once "../utility/database_connect.php";
    require_once "../utility/room_management.php";
    require_once "../utility/player_management.php";

    class Player implements Serializable{
        public $username;
        public $soldi;
        public $bet; //contiene la puntata della mano corrente
        public $status;
        public $cards;

        public function __construct($player_info = null){
            if(!$player_info)
                return;
            $this->username = $player_info['username'];
            $this->soldi = $player_info['soldi'];
            $this->bet=0;
            $this->status = "wait"; //wait | raise -> highest | fold | done | allin
            //stati possibili:
            //wait: attesa di giocare
            //raise: non ha eguagliato la puntata massima
            //highest: ha giocato la puntata massima
            //done: sta aspettando finisca il turno
            //fold, allin
            $this->cards = [];
        }

        public function takeCards(&$deck){
            for($i=0;$i<2;$i++)
                $this->cards[]=array_pop($deck);
        }

        public function serialize(){
            return serialize([
                'u'=>$this->username,
                's'=>$this->soldi,
                'b'=>$this->bet,
                't'=>$this->status,
                'c'=>$this->cards
            ]);
        }

        public function unserialize($serialized_player){
            $player = unserialize($serialized_player);
            $this->username = $player['u'];
            $this->soldi = $player['s'];
            $this->bet = $player['b'];
            $this->status = $player['t'];
            $this->cards = $player['c'];
        }
    }

    class Room implements Serializable{

        public $highest_bet; //puntata piu alta del round
        public $round; //round corrente
        public $first_unassigned_id; //id non assegnato (corrisponde al numero di giocatori)
        public $deck;
        public $player_list;
        public $house_cards; //carte del dealer
        public $total_bet; //vincita totale

        public function __construct($room_info = null){
            if(!$room_info || !$room_info['stato_partita'] || $room_info['stato_partita']==="-" ){ //la stanza è nuova
                $this->highest_bet=0;
                $this->round=1;
                $this->first_unassigned_id=0;
                $this->player_list=[];
                $this->house_cards=[];
                $this->total_bet=0;
                $this->inizializzaDeck();
            }else{
                $this->unserialize($room_info['stato_partita']);
            }
        }

        private function getId(){
            $id = $this->first_unassigned_id;
            ++$this->first_unassigned_id;
            return $id;
        }

        public function findNextTurn($current){
            $next = $current;
            for($i=0;$i<$this->first_unassigned_id;$i++){
                $next = ($next+1)%$this->first_unassigned_id;
                //if(isset($this->player_list[$next])){ //controllo ridondante. da rimuovere
                    $player= $this->player_list[$next];
                    if($player->status !== "allin" && $player->status !=="fold" && $player->status !=="done" && $player->status !=="highest")
                        break;
                //}
            }
            return $next;
        }

        public function addPlayer($player_info){
            $player = new Player($player_info);
            $id = $this->getId();
            $this->player_list[$id]=$player;
            return $id;
        }

        public function checkPlayers(){
            $players['ingame']=0;
            $players['played']=0;
            $players['allin']=0;
            foreach($this->player_list as $player){ //itero la stanza per contare il numero di giocatori nei vari stati
                if($player->status !=="fold"){
                    if($player->status === "allin")
                        $players['allin']++;  
                    else{
                        $players['ingame']++;
                        if($player->status === "done" || $player->status === "highest")
                            $players['played']++;
                    } 
                }
            }
            return $players;
        }

        private function inizializzaDeck(){ //creo il deck e lo mischio
            $this->deck=[];
            $cards = ["A","2","3","4","5","6","7","8","9","10","Q","J","K"];
            $seeds = ["H","D","C","S"];
            foreach($seeds as $s){
                foreach($cards as $c){
                    $this->deck[]=$c.$s;
                }
            }
            shuffle($this->deck);
        }
        
        public function giveCards(){ //da le carte a tutti i giocatori
            foreach($this->player_list as $player)
                $player->takeCards($this->deck);
        }

        public function dealHouseCard(){ //estrae una carta per il dealer
            $card = array_pop($this->deck);
            $this->house_cards[]=$card;
            return $card;
        }

        public function getAllCards(){ //estrae tutte le carte per inviarle ai giocatori alla fine di una mano
            $all_cards = [];
            foreach($this->player_list as $id => $pl){
                $all_cards[$id]=$pl->cards;
            }
            return $all_cards;
        }

        public function getPot(){ // estrae il 5% dei soldi da ogni giocatore e lo aggiunge alla vincita totale, analogamente alla modifica eseguita sul database
            foreach($this->player_list as $id => $player){
                $pot = floor($player->soldi*0.05);
                $this->total_bet += $pot;
                $player->soldi -= $pot;
            }
            if($_SESSION['soldi']>0)
                $_SESSION['soldi'] -= floor($_SESSION['soldi']*0.05);
        }

        public function setEverythingUp(){
            $this->giveCards();
            $this->dealHouseCard();
            $this->getPot();
        }

        public function checkWinner(){

            $fold_win =false;
            $pl = $this->player_list;
            $house_cards = $this->house_cards;
            static $score = [
                'SCALA REALE'=>10, 'SCALA COLORE'=>9, 'POKER' => 8,
                'FULL'=>7, 'COLORE' =>6, 'SCALA' => 5, 'TRIS' => 4,
                'DOPPIA COPPIA'=> 3, 'COPPIA'=>2, 'ALTA'=>1
            ];
            $first = array_key_first($pl);
            $rank_winner = -1;
            $id_winner = -1;
            
            $w_hand=[];
            if(count($this->house_cards)<3)
                $fold_win=true;
            foreach($pl as $i => $contender){
                if($contender->status==="fold")
                    continue;
                if($fold_win){ // se ci sono meno carte del previsto, non eseguo il calcolo, hanno foldato tutti tranne 1
                    $id_winner=$i;
                        $w_hand['COMBINAZIONE']="FOLD";
                    break;
                }
                //unisco le carte del giocatore e del banco e calcolo le combinazioni
                $c_hand =evaluateCards($house_cards,$pl[$i]->cards);

                $rank_contender=$score[$c_hand['COMBINAZIONE']]; //traduco la combinazione in punteggi
                if($rank_contender===$rank_winner){ //parte lo spareggio
                    $dethroned=false;
                    $cls=$c_hand['ls_card']; //estraggo la carta uscita piu spesso
                    $css=$c_hand['ss_card']; //e la seconda carta uscita piu spesso per i confronti
                    $wls=$w_hand['ls_card'];
                    $wss=$w_hand['ss_card'];
                    $contender_has_higher = compareAlta($c_hand,$w_hand);
                    switch ($c_hand['COMBINAZIONE']) {
                        case 'COPPIA': //se coppia/tris, metto a confronto la carta delle combinazioni, e se sono uguali guardo la carta alta
                        case 'TRIS':
                            if($cls>$wls || ($cls===$wls && $contender_has_higher))
                                $dethroned=true;
                            break;
                        case 'DOPPIA COPPIA':
                            $c_pairs = [$cls,$css]; //per doppia coppia, guardo prima la coppia piu alta, e se uguali la seconda coppia
                            $w_pairs = [$wls,$wss]; //se uguali la carta alta
                            rsort($c_pairs);
                            rsort($w_pairs);
                            if($c_pairs[0]>$w_pairs[0] || 
                              ($c_pairs[0]==$w_pairs[0] && 
                              ($c_pairs[1]>$w_pairs[1] || 
                              ($c_pairs[1]==$w_pairs[1] && $contender_has_higher))))
                                $dethroned=true;
                            break;
                        case 'FULL': //per il full guardo prima il tris, poi la coppia, poi nel caso carta alta
                            if($cls>$wls || 
                              ($cls===$wls && 
                              ($css>$wss || 
                              ($css===$wss && $contender_has_higher))))
                                $dethroned=true;
                            break;

                        default : //ALTA,COLORE,SCALA,SCALA COLORE guardo solo la carta alta
                            if($contender_has_higher)
                                $dethroned=true;
                            break;
                        //scala reale e poker non possono pareggiare
                    }
                    if(!$dethroned) //decremento il perdente in modo da far si che perda il prossimo confronto
                        $rank_contender--;
                    else
                        $rank_winner--;
                }
                if($rank_contender>$rank_winner){
                    $rank_winner=$rank_contender;
                    $id_winner=$i;
                    //$winner_combination = $contender_combination;
                    $w_hand=$c_hand;
                }
            }
            return ['id'=>$id_winner,'comb'=>$w_hand['COMBINAZIONE']];
        }

        public function serialize(){
            $serialized_players = [];
            foreach($this->player_list as $key =>$player) //prima serializzo i giocatori individualmente
                $serialized_players[] = [
                    'k'=>$key,
                    'p'=>$player->serialize()
                ];

            return serialize([
                'b'=> $this->highest_bet,
                'r'=> $this->round,
                'i'=> $this->first_unassigned_id,
                'd'=> $this->deck,
                'h'=> $this->house_cards,
                't'=> $this->total_bet,
                'p'=> $serialized_players
            ]);
        }
        public function unserialize($serialized_state){
            $state = unserialize($serialized_state);
            $this->highest_bet = $state['b'];
            $this->round = $state['r'];
            $this->first_unassigned_id = $state['i'];
            $this->deck = $state['d'];
            $this->house_cards = $state['h'];
            $this->total_bet = $state['t'];

            if($state['p']){ //Deserializzo individualmente i giocatori
                foreach($state['p'] as $serialized_player){
                    $player = new Player();
                    $player->unserialize($serialized_player['p']);
                    $this->player_list[$serialized_player['k']] = $player;
                }
            }else
                $this->player_list=[];
        }
    }

    //classe che viene usata per contenere la storia delle mosse
    class Storia implements Serializable{
        public $next_move_id; //indice globale utilizzato per permettere ai giocatori di estrarre solo le mosse che non hanno visto
        public $moves;
        
        public function __construct($serialized_state=null){
            if(!$serialized_state || $serialized_state==="-"){
                $this->next_move_id=0;
                $this->moves=[];
            }else{
                $this->unserialize($serialized_state);
            }
        }
        
        public function setNewMove($who,$what){
            if($this->next_move_id>15)
                array_shift($this->moves); //se ho piu di 15 mosse, estraggo l ultima
            $this->next_move_id++; //questo valore viene solo incrementato, non viene mai decrementato
            $this->moves[]=[$who,$what];
        }

        public function getLastMoves(){
            if(!isset($_SESSION['NEXT_MOVE']))
                $_SESSION['NEXT_MOVE']=0;
            if($_SESSION['NEXT_MOVE']!== $this->next_move_id){ //se ho mosse da leggere,estraggo solo quelle mosse dal vettore
                $moves = array_slice($this->moves,-($this->next_move_id-$_SESSION['NEXT_MOVE'])); 
                $_SESSION['NEXT_MOVE']=$this->next_move_id;
                return $moves;
            }else
                return [];
        }
        
        public function serialize(){
            return serialize([
                'n'=>$this->next_move_id,
                'm'=>$this->moves
            ]);
        }

        public function unserialize($serialized_history){
            $moves_history = unserialize($serialized_history);
            $this->next_move_id = $moves_history['n'];
            $this->moves = $moves_history['m'];
        }
    }

    function readMoves($serialized_history){ //funzione wrapper per facilitare la lettura delle mosse
        $moves=[];
        if($serialized_history !="-"){
            $moves_history = new Storia($serialized_history);
            $moves = $moves_history->getLastMoves();
        }
        return $moves;
    }

    //esegue le operazioni associate alla mossa e restituisce la mossa nel formato letto dai client
    function checkMoveOutcome($type,$amount,Room $room,Player $player){ 
        $bet = 0;
        $move = [];
        $soldi = $player->soldi;

        if($type==="check"){ //eguaglia la puntata piu alta
            $bet = ($room->highest_bet-$player->bet)<$soldi ? ($room->highest_bet-$player->bet) : $soldi;
            $player->status="done";
            $move=["check",$bet];
        }else if($type==="bet"){
            
            if($amount<=0){  //non posso scommettere un valore negativo
                $_SESSION['is_being_served'] = false;
                return false;
            }
            $bet = $amount < $soldi ? $amount : $soldi; //prendo il valore piu piccolo tra i miei soldi e la scommessa

            if($room->highest_bet < $bet + $player->bet){ //se la mia scommessa complessiva è piu alta dell'ultima piu alta, diventa la piu alta 
                $room->highest_bet = $bet + $player->bet;
                $player->status = "highest";
                foreach($room->player_list as $other_player){
                    if($player===$other_player)
                        continue;
                    if($other_player->status !== "fold" && $other_player->status !== "allin"){
                        $other_player->status = "raise"; //segnala ai giocatori che devono eguagliare la puntata piu alta per continuare a giocare
                    }
                }
            }
            $move=["bet",$bet];
        }else if($type==="fold"){
            $player->status="fold";
            $move=["fold"];
        }else{
            $_SESSION['is_being_served'] = false;
            return false;
        }

        if($type!=="fold" && $bet > 0){ //aggiorno i soldi nel caso di un eventuale variazione
            $soldi -= $bet;
            $player->soldi = $soldi;
            $player->bet += $bet;
            $room->total_bet += $bet;

            if($soldi == 0)
                $player->status = "allin";
            
        }
        return $move;
    }

    function checkGameStatus(Room $room, Storia $moves_history = null){
        $status = $room->checkPlayers(); //ottengo il numero di giocatori nei vari stati
        $is_over = false;
        $dealer_cards = null;

        if($status['ingame'] + $status['allin']== 1){ //vero solo se uno dei due vale 1 e l'altro vale 0
            $is_over=true;

        }else if($status['ingame']===$status['played']){ //se hanno giocato tutti

            if($status['ingame']<=1 && $status['allin']>0){ //se c'è almeno uno che è all in e massimo un unico giocatore rimasto che non ha ne foldato ne fatto allin
                
                while(count($room->house_cards)<3){ //distribuisco le carte del dealer che mancano
                    $hcard=$room->dealHouseCard();

                    if($moves_history)//if che separa gestione delle mosse da singleplayer a multiplayer
                        $moves_history->setNewMove('d',$hcard);
                    else
                        $dealer_cards[]=$hcard;
                }
                $is_over=true;
            }else{
                $room->round++;
                if($room->round>3){
                    $is_over=true; //se ho fatto 3 round la partita è finita
                }else{
                    $room->highest_bet=0; //altrimenti resetto le informazioni necessarie, come scommessa piu alta e stato dei giocatori
                    foreach($room->player_list as $temp_player){
                        $temp_player->bet=0;
                        if($temp_player->status !== "fold" && $temp_player->status !== "allin"){
                            $temp_player->status = "wait";
                        }
                    }
                    
                    $hcard = $room->dealHouseCard(); //estraggo la nuova carta del dealer
                    if($moves_history) //if che separa gestione delle mosse da singleplayer a multiplayer
                        $moves_history->setNewMove('d',$hcard);
                    else
                        $dealer_cards[]=$hcard;
                }
            }
        }
        return ['over'=>$is_over,'deal'=>$dealer_cards];
    }

    //funzione che aggiorna tutte le informazioni e il turno con un unica query sfruttando l'atomicità delle operazioni sql sulla riga della stanza
    function setStateAndTurn($room_id,$player_id = -1,Room $room = null,Storia $moves_history = null){
        $connection = db_connect();
        if($room){ //se ho una stanza inizializzata, estraggo il prossimo giocatore e serializzo la struttura dati
            $next = $room->findNextTurn($player_id);
            $room_state = $room->serialize();
        }else{
            $next = 0;
            $room_state = "-";
        }

        if($moves_history != null)
            $serialized_history = $moves_history->serialize(); 
        else
            $serialized_history="-";

        if($_SESSION['soldi'] === $_SESSION['soldi_old']){ //se non sono cambiati i miei soldi, aggiorno solamente le informazioni della stanza
            $query = mysqli_prepare($connection,"UPDATE stanze SET storia_mosse=?,stato_partita=?,turn=?,turn_timestamp = NOW() WHERE ID=?");
            mysqli_stmt_bind_param($query,"ssss",$serialized_history,$room_state,$next,$room_id);

        }else{ //è sempre il mio turno se soldi viene modificato
            $query = mysqli_prepare($connection,"UPDATE utenti u INNER JOIN stanze s on s.ID=u.stanza SET soldi=?, storia_mosse=?,stato_partita=?,turn=?,turn_timestamp = NOW() WHERE username = ?");
            mysqli_stmt_bind_param($query,"issss",$_SESSION['soldi'],$serialized_history,$room_state,$next,$_SESSION['username']);
            $_SESSION['soldi_old']=$_SESSION['soldi'];
        }

        if(!mysqli_stmt_execute($query) || mysqli_stmt_affected_rows($query)<1){
            $result = -1;
        }else{
            $result = $next; //restituisco l'id del prossimo giocatore
        }
        mysqli_stmt_close($query); 
        mysqli_close($connection);
        return $result;
    }

    function updateWinnerMoney($player,$winnings){ //funzione chiamata nel caso io non sia il vincitore (in quel caso ci pensa setStateAndTurn() ad aggiornare i soldi)
        $username = $player->username;              // utilizzo una funzione separata per il caso in cui il vincitore esce dalla stanza prima della vincita, dunque il join
        $player->soldi += $winnings;                // con la tupla della stanza fallirebbe e non aggiornerei le informazioni
        $connection = db_connect();

        $query = mysqli_prepare($connection,"UPDATE utenti SET soldi = soldi+? WHERE username = ? AND ( stanza = ? OR stanza = NULL)");
        mysqli_stmt_bind_param($query,"iss",$winnings,$username,$_SESSION['ROOM_ID']);
        
        mysqli_stmt_execute($query);
        mysqli_stmt_close($query); 
        mysqli_close($connection);
    }

    function traduciCarta($card){ //traduce la carta e il seme in un valore numerico
        $seme = substr($card, -1);
        $numero = substr($card, 0, -1);
        static $cardmap = [
            '2'=>2, '3'=>3, '4'=>4, '5'=>5,
            '6'=>6, '7'=>7, '8'=>8, '9'=>9,
            '10'=>10, 'J'=>11, 'Q'=>12, 'K'=>13, 'A'=>14
        ];
        static $seedmap = [
            'C'=>19, 'D'=>13, 'H'=>7, 'S'=>3
            ];
    return [$cardmap[$numero], $seedmap[$seme]];
    }

    function normalizzaMano($hand){ //traduce ogni carta in valore numerico poi ordina la mano in ordine decrescente
        $tradotte = [];             //in modo da facilitare il calcolo delle combinazioni
        foreach ($hand as $c) {
            $tradotte[] = traduciCarta($c);
        }

        usort($tradotte, function($a, $b) {
            return $b[0] <=> $a[0];
        });
        return $tradotte;
    }

    function initCombinazioni(){ //vettore di combinazioni, verrà manipolato da evaluateCards 
        return [
            'SCALA REALE'=>true, 'SCALA COLORE'=>true, 'POKER' => false,
            'FULL'=>false, 'COLORE' =>true, 'SCALA' => true, 'TRIS' => false,
            'DOPPIA COPPIA'=> false, 'COPPIA'=>false
        ];
    }

    function evaluateCards($house_cards,$player_cards){

        $risultato = [];

        $valore=0; //indici per leggibilità
        $seme=1;

        $my_cards = normalizzaMano($player_cards); //traduco e riordino le carte in modo decrescente
        $risultato['ALTA'] = $my_cards[0][$valore]; //prendo la carta alta dalle carte private
        $risultato['seme'] = $my_cards[0][$seme];
        $risultato['BASSA'] = $my_cards[1][$valore]; //mi segno anche la bassa per tiebreaker

        $hand = array_merge($house_cards,$player_cards);


        $carte = normalizzaMano($hand);
        $c_prec = null; //tengo conto della carta precedente

        $combinazioni = initCombinazioni(); //creo il vettore di combinazioni

        $carte_diverse = 1;     //numero di carte con NUMERO diverso all'interno della mano
        $longest_streak = 1;    //numero di carte di fila piu alto (essendo ordinato in maniera decrescente equivale a quante carte di un tipo ci sono)
        $longest_streak_card=0;     //quale carta compare piu volte
        $snd_streak_card=0;     //numero di carte di fila NON piu alto
        $streak = 1;        //quante carte di fila sono comparse fino a quel momento
        $asso_basso = false;
        //$risultato['hand value']=0; //valore della mano, per semplificare i casi con i pareggi
        
        foreach($carte as $c){
            //$risultato['hand value'] += $c[$valore]*$c[$seme]; //calcolo il valore della mano come NUMERO * VALORE NUMERICO DEL SEME sommato per ogni carta
            if($c[$valore] < 10)
                $combinazioni['SCALA REALE']=false;
            
            if($c_prec){
                if($c[$seme]!==$c_prec[$seme])
                    $combinazioni['COLORE']=false; //se ho 2 colori diversi SICURAMENTE non ho colore
                if($c[$valore]!==$c_prec[$valore]){
                    $carte_diverse++;
                    $streak=1;
                }else{
                    $streak++;
                    if($longest_streak<=$streak){
                        $snd_streak_card=$longest_streak_card; //salvo la ex streak piu lunga in snd_longest_streak
                        $longest_streak=$streak;
                        $longest_streak_card=$c[$valore];
                    }
                }
                //se le carte non hanno un valore decrescente sicuramente non ho scala
                //tranne nel caso asso basso => A, 5, 4, 3, 2
                if($c[$valore]!==$c_prec[$valore]-1 && $combinazioni['SCALA']){ 
                    if($c_prec[$valore]===14 && $c[$valore] ===5)
                        $asso_basso=true;
                    else
                        $combinazioni['SCALA']=false;
                }
            }
            $c_prec=$c;
        }
            
        if(!$combinazioni['COLORE'] || !$combinazioni['SCALA']){
            $combinazioni['SCALA REALE']=false;
            $combinazioni['SCALA COLORE']=false;
        }
        switch ($carte_diverse) {
            case 2:
                if($longest_streak===3)
                    $combinazioni['FULL']=true;
                $combinazioni['POKER'] = !$combinazioni['FULL'];
                break;
            case 3:
                if($longest_streak===3)
                    $combinazioni['TRIS']= true;
                else
                    $combinazioni['DOPPIA COPPIA'] = true;
                break;
            case 4: //se ho 4 carte diverse ho sicuramente coppia
                $combinazioni['COPPIA'] = true;
                break;
        }
        $comb = array_search(true,$combinazioni); //prendo la combinazione piu alta
        $risultato['COMBINAZIONE'] = $comb !== false ? $comb : 'ALTA'; 
        if($combinazioni['SCALA'] && $asso_basso && $risultato['ALTA'] == 14){ //nel caso dell'asso basso, se ho l asso nella mia mano, lo faccio valere 1
            $risultato['BASSA'] = $risultato['ALTA'];
            $risultato['ALTA'] = $my_cards[1][$valore]; //prendo la carta alta dalle carte private
            $risultato['seme'] = $my_cards[1][$seme];
        }
        $risultato['ls_card']=$longest_streak_card;
        $risultato['ss_card']=$snd_streak_card;

        return $risultato;
    }

    //true se vince contender, false se vince winner
    function compareAlta($c_hand,$w_hand){      //confronta la carta piu alta, se uguali confronta la bassa, altrimenti i semi della piu alta
        return ($c_hand['ALTA']>$w_hand['ALTA'] ||
                ($c_hand['ALTA']===$w_hand['ALTA'] && 
                ($c_hand['BASSA']>$w_hand['BASSA'] || 
                ($c_hand['BASSA']===$w_hand['BASSA'] &&
                $c_hand['seme']>$w_hand['seme']))));
    }

    function hijackSession($full_info){ // impersono un giocatore per farlo foldare
        if(isset($_SESSION['MY_ID']))
            $_SESSION['TRUE_ID'] = $_SESSION['MY_ID'];
        $_SESSION['TRUE_username'] = $_SESSION['username'];

        $state=unserialize($full_info['stato_partita']);
        $_SESSION['MY_ID'] = $full_info['turn'];
        $room = new Room($full_info);
        $_SESSION['username'] = $room->player_list[$full_info['turn']]->username;
        $_GET['move']="fold";
        unset($_GET['info']);
    }

    function restoreSession(){ //ripristino le mie informazioni
        if(isset($_SESSION['TRUE_ID']))
            $_SESSION['MY_ID'] = $_SESSION['TRUE_ID'];
        $_SESSION['username'] = $_SESSION['TRUE_username'];
        unset($_SESSION['TRUE_ID']);
        unset($_SESSION['TRUE_username']);
    }
?>