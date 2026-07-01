'use strict';
let {isCreator,username} = window.appData;
let my_id=-1;
let myself = null;
let dealer;
let my_total_bet=0;

let total_bet=0;
let highest_round_bet=0;
const all_positions = ["bottom","right","top","left"];
let player_map = []; // serve per tradurre i messaggi del server
let moves_buffer = [];
let turn = -999;

const speed = ["SLOW","FAST","FASTER"];
let speed_mode =0;

const check_type = ["CHECK","CALL"];
const bet_type =["BET","RAISE"];
let button_mode = 0;

let received_game_info=false;
let is_running=false;
let is_playing=false;

let game_over="";
let entry;

let timer;
let time_to_wait = 0;
let timer_value;
let timer_interval=null;

const TIMEOUT = 30;
const WAITING_TIME = 1000;
const CARDS_SHOW_DELAY = 8000;
const MOVES_DELAY = 1000;
const OUTPUT_MESSAGE_DURATION = 5000;
const CARD_FOLDER = "/Assets/cards/";
const PFP_FOLDER = "/Assets/profilepictures/";
const IMG_EXT = ".png";

async function requestJson(data){
    const response = await fetch(data);
    if(!response.ok)
        throw new Error(`Errore HTTP: ${response.status}`);
    else{
        const json = await response.json();
        return json;
    }
}

function sleep(ms,ignore_speed = false){
    if(!ignore_speed){
        switch (speed_mode) {
            case 1:
                ms = Math.floor(ms/2);
                break;
                
            case 2:
                ms = 0;
                break;
                
            default:
                break;
        }
    }
    return new Promise((resolve)=>setTimeout(resolve,ms));
}

function cardPath(card){
    return CARD_FOLDER + card + IMG_EXT;
}

function hideEverything(){
    document.getElementById('HUD').hidden=true;
    document.getElementById('total_bet').hidden=true;
    for(let pl of player_map){
        pl.div.hidden=true;
    }
    player_map=[];

    document.querySelectorAll('[class=player_info]').forEach(elem => elem.innerHTML="");
    document.querySelectorAll('.card_container img').forEach(carta=>{carta.style.visibility="hidden"; carta.src = cardPath("backcarta")});
    document.querySelectorAll('.player_profile img').forEach(pfp=> pfp.classList="");
}

function showMessage(messaggio,until=0){
    entry.textContent=messaggio;
    entry.hidden=false;
    if(until>0){
        setTimeout(()=>entry.hidden=true,until*1000);
    }
}

async function playGiveCardsAnimation(interval){
    const cards = document.querySelectorAll('.main_container:not([hidden]) .card_container img');
    for(let c of cards){
        c.style.visibility="visible";
        if(interval)
            await sleep(interval);
    }
}

async function addPlayerToTable(player_id,position,player_data,pfp_path){
    let player_username= player_data['u'];
    let player_soldi= player_data['s'];

    if(!all_positions.length)
        return;

    const new_player = document.getElementById("player_"+position);
    player_map[player_id]={
            pos: position,
            username: player_username,
            soldi: player_soldi,
            rnd_bet: 0,
            div: new_player
            };

    const player_info = new_player.getElementsByClassName('player_info')[0];
    const classes = ["username","soldi","status"];
    const txt = [player_username,"Chips: "+player_soldi,""];
        
    for(let i = 0; i<3;i++){
        const p = document.createElement('p');
        p.textContent=txt[i];
        p.classList="player_info "+classes[i];
        player_info.appendChild(p);
    }
    if(player_id!=my_id){
        const pfp = new_player.querySelector('img[alt=pfp]');
        pfp.src = PFP_FOLDER + ((pfp_path == '-') ? "default" : pfp_path) + IMG_EXT;
    }
    new_player.hidden = false;
    if(position==="bottom")
        document.getElementById('HUD').hidden=false;
}

function changeSpeed(e){
    speed_mode = (speed_mode + 1) % 3;
    e.target.textContent = speed[speed_mode];

}

function updateButtonText(){
    const check = document.getElementById('check');
    const bet = document.getElementById('bet');

    check.textContent = check_type[button_mode];
    bet.textContent = bet_type[button_mode];
    bet.classList = ((button_mode===0)? "bet" : "raise") + " darken";
}

function checkButton(){
    clearTimer();
    returnBetToNormal();
    changeButtonState();
    waitForInfo("check");
}

function betButton(e){
    if(highest_round_bet >= myself.soldi)
        return;
    const bet = e.target;
    const betamount = document.getElementById('betamount');
    if(betamount.value===""){
        betamount.value="";
        bet.textContent = (bet.textContent ===bet_type[button_mode]) ? "CANCEL "+ bet_type[button_mode] : bet_type[button_mode];
        betamount.style.visibility= (betamount.style.visibility !== "visible") ? "visible" : "hidden";
        return;
    }
    clearTimer();
    waitForInfo("bet",betamount.value);
    returnBetToNormal();
    changeButtonState();
}

function checkBetAmount(e){
    const betamount = e.currentTarget;
    const bet = document.getElementById('bet');

    if(betamount.value===""){
        bet.textContent = "CANCEL "+ bet_type[button_mode];
        bet.disabled=false;
        betamount.setCustomValidity("");
    }else{
        let amount = parseInt(betamount.value);
        bet.textContent = "SEND "+ bet_type[button_mode];
        if(amount > myself.soldi){
            bet.disabled=true;
            betamount.setCustomValidity("Non hai abbastanza soldi");
        }else if((amount + myself.rnd_bet) <= highest_round_bet && myself.soldi > (highest_round_bet - myself.rnd_bet)){
            bet.disabled=true;
            betamount.setCustomValidity("La puntata è troppo bassa");
        }else{
            bet.disabled=false;
            betamount.setCustomValidity("");
            if(amount == myself.soldi){
                bet.classList = "allin darken";
                bet.textContent = "ALL-IN";
            }else
                bet.classList = (button_mode===0)? "bet" : "raise";
        }
    }
}

function returnBetToNormal(){
    const bet = document.getElementById('bet');
    
    if(bet.textContent===bet_type[button_mode])
        return;
    const betamount = document.getElementById('betamount');

    betamount.style.visibility = "hidden";
    betamount.value="";
    betamount.setCustomValidity("");

    bet.textContent=bet_type[button_mode];
    bet.disabled=false;
}

function foldButton(){
    clearTimer();
    returnBetToNormal();
    changeButtonState();
    is_playing=false;
    waitForInfo("fold");
}

function sendStartSignal(){
    document.getElementById('start_button').hidden=true;
    entry.hidden=true;
    waitForInfo();
}

function changeButtonState(){
    document.getElementById('moves').querySelectorAll('button').forEach(bottone=>bottone.disabled= !bottone.disabled);
}

function timerBody(send_fold) {
    if(timer_value>=0){
        timer.textContent=timer_value--;
        timer_interval=setTimeout(timerBody.bind(null,send_fold),1000)
    }
    else{
        if(!is_running && send_fold)
            foldButton();
        timer.textContent="";
    }
}

function startTimer(send_fold){
    if(!timer_interval){
        timer_value = time_to_wait;
        timer_interval=setTimeout(timerBody.bind(null,send_fold), 1000);
    }
}

function clearTimer(){
    clearTimeout(timer_interval);
    timer_interval=null;
    timer.textContent="";
    time_to_wait = TIMEOUT;
}

function removeAndAddGlow(who){
    const prev =document.querySelector('.player_profile img[class=isturn]');
    if(prev)
        prev.classList.remove('isturn');
    const new_pfp = player_map[who].div.querySelector('.player_profile img');
    new_pfp.classList="isturn";
    const p=player_map[who].div.querySelector('.player_info .status');
    if(who != my_id)
        p.textContent="HIS TURN";
    else
        p.textContent="YOUR TURN";
}

function changeTurn(next){
    if(next >= 0){ //se il turno è valido, lo cambio
        if(next===my_id)
            changeButtonState();
        //rimuovo il contorno verde a chi lo aveva prima e lo do al giocatore che ha il turno
        removeAndAddGlow(next);
        startTimer((next == my_id)?true:false);
        turn = next;
    }
}

async function showAllCards(all_cards){
    for( const [id,cards] of all_cards.entries()){ //sostituisco tutte le carte coperte con quelle scoperte 
        let pl_cards = player_map[id].div.querySelectorAll('.card_container img');
        pl_cards.forEach((card,num)=> card.src=cardPath(cards[num]));
    }
    await sleep(CARDS_SHOW_DELAY,true);
}

async function updateStatus(move){ //aggiorna lo stato della partita eseguendo l animazione di una mossa
    const [autore,this_move]=move; //move[0] contiene l' autore della mossa, move[1] la mossa in se
    switch (autore) { 

        case 'd': //se dealer
            button_mode=0; 
            updateButtonText(); //resetto le scritte sui bottoni

            highest_round_bet=0; //resetto le informazioni del round
            for(let pl of player_map)
                pl.rnd_bet=0;
            let my_board = document.getElementById('my_bet_board');
            my_board.textContent="";
            let need_board = document.getElementById('needed_bet_board');
            need_board.textContent="";
            const card = document.createElement('img');
            card.src = cardPath(move[1]);
            card.alt = "card";
            if(!received_game_info)
                card.style.visibility="hidden"; //necessario per il caso in cui si crea la carta prima di eseguire playGiveCardsAnimation()
            else
                card.style.visibility="visible";
            dealer.cards.appendChild(card); //aggiungo la carta al dealer
            break;

        default: //se giocatore
            if(!(autore in player_map))
                break;
            let player = player_map[autore].div;
            //al massimo aggiungi aureola?
            let p_soldi =player.querySelector('.player_info .soldi');
            let p_status = player.querySelector('.player_info .status');
            switch (this_move[0]) { //indica il tipo della mossa
                case 'win': //se vincitore
                    const winner = player_map[autore];
                    let wininfo=move[1];
                    let output = "";
                    await showAllCards(wininfo[2]);
                    if(myself && (myself.soldi === 0 && (total_bet===0 || winner !== myself))){
                        game_over="HAI FINITO I SOLDI";
                        output=game_over;
                    }else{
                        output= winner.username + " HA VINTO "+ total_bet;
                        output = output+ ((wininfo[1]=="FOLD")?" PER ABBANDONO":" CON " +((wininfo[1]=="ALTA")?"CARTA " : "") +wininfo[1]);

                    }
                    //resetto le informazioni
                    total_bet=0;
                    my_total_bet = 0;
                    dealer.cards.innerHTML="";
                    received_game_info=false;
                    my_id=-1;
                    turn= -999;
                    time_to_wait=TIMEOUT;
                    
                    hideEverything();
                    showMessage(output,(game_over=="")?OUTPUT_MESSAGE_DURATION:0);
                    await sleep(OUTPUT_MESSAGE_DURATION,true);
                    break;

                case "check": //check ha anche la funzione di call
                case "bet": //bet ha anche la funzione di raise
                    removeAndAddGlow(autore);
                    let first_bet=false;
                    let is_raise=false;
                    let betamount = parseInt(this_move[1]);
                    let message = "";
                    let need_board = document.getElementById('needed_bet_board');
                    if(betamount>0){
                        if(highest_round_bet==0)
                            first_bet=true;

                        player_map[autore].soldi -= betamount;
                        player_map[autore].rnd_bet += betamount;
                        p_soldi.textContent = "Chips: "+player_map[autore].soldi; //mostro le variazioni delle informazioni del giocatore
                        
                        let bet_board = document.getElementById('bet_board');
                        let my_board = document.getElementById('my_bet_board');
                        if(total_bet===0)
                            document.getElementById('total_bet').hidden=false; // mostro la vincita totale
                        
                        if(highest_round_bet < player_map[autore].rnd_bet){
                            is_raise=true;
                            highest_round_bet= player_map[autore].rnd_bet;
                            if(autore !== my_id && is_playing){
                                need_board.textContent= `To match: ${highest_round_bet-player_map[my_id].rnd_bet}`;
                            }else
                                need_board.textContent="";
                        }
                        
                        total_bet += betamount;
                        bet_board.textContent=`Total bet: ${total_bet}`;
                        if(autore === my_id){ //aggiorno le mie informazioni per il controllo sull'input
                            my_total_bet+=betamount;
                            my_board.textContent = (myself.rnd_bet == 0) ? "" : `Your Bet: ${myself.rnd_bet}`;
                        }
                        if(is_raise){
                            button_mode = 1;
                            updateButtonText();
                        }
                    }
                    if(this_move[0]==="check"){
                        message = check_type[button_mode] +"ED"; 
                        need_board.textContent="";
                    }else{
                        if(first_bet)
                            message = "BET";
                        else
                            message = "RAISED";
                        }
                    if(betamount>0)
                        message = message +": " +betamount;
                    p_status.textContent=message;
                    break;
                
                case "fold":
                    player.querySelector('.player_profile img').classList="folded"; //aggiungo il bordo rosso al giocatore che ha foldato
                    player.querySelectorAll('.card_container img').forEach(carta => carta.style.visibility="hidden");
                    p_status.textContent="FOLDED";
                    p_status.style.color="red";
                    break;
            }
            break;
    }
}

async function playMoves(){
    const movlen=moves_buffer.length;
    if(!movlen)
        return;

    let stop_index = movlen;
    for(let i=0;i<movlen;i++){ 
        await updateStatus(moves_buffer[i]); 

        // faccio solo le mosse fino ad un eventuale vincita, quelle rimanenti le faccio dopo aver richiesto nuovamente informazioni al server
        if(moves_buffer[i][0]!=='d' && moves_buffer[i][1][0]==="win"){ 
            stop_index=i+1;
            break;
        }
        await sleep(MOVES_DELAY);
    }
        moves_buffer.splice(0, stop_index); //elimino le mosse eseguite
}

function inizializzaStrutturePartita(risposta){
    let nplayers=risposta.list.length;
        
    for(let i=0;i<nplayers;i++){
        if(username === risposta.list[i]['u']){
            my_id=i; //salvo il mio indice
        }
    }
    let index = ((-my_id) % 4 + 4 ) % 4; //calcolo l'indice di partenza in modo da avere posizioni relative identiche nella sessione di ogni giocatore
    for(let i=0;i<nplayers;i++){
        addPlayerToTable(i,all_positions[index],risposta.list[i],risposta.pfps[i]);
        index=(index+1)%4;
    }
    if(my_id>=0) //posso non avere un id se sto guardando una partita in corso
        myself=player_map[my_id];
        
    total_bet = risposta.bet;
    highest_round_bet = risposta.highest;
    my_total_bet = risposta.my_bet;

    if(total_bet!=0){
        let bet_board = document.getElementById('bet_board');
        bet_board.textContent=`Total bet: ${total_bet}`;
        document.getElementById('total_bet').hidden=false; // mostro la vincita totale
    }
    for(let i=0;i<risposta.deal.length;i++){
        moves_buffer.push(['d',risposta.deal[i]]);
    }
    if('cards' in risposta){ //posso non avere carte
        const cards = myself.div.querySelectorAll('.card_container img');
        for(let i=0;i<2;i++){
            cards[i].src=cardPath(risposta.cards[i]);
        }
        is_playing=true;
    }else{
        is_playing=false; //in questo caso mi comporto da spettatore
        showMessage("Gli altri giocatori stanno terminando il round",4);
    }
    if(!time_to_wait)
        time_to_wait = TIMEOUT - (('time' in risposta)? risposta.time: 0);

    const backcard = document.createElement('img');
    backcard.src = cardPath("backcarta");
    backcard.alt="card";
    backcard.style.visibility="hidden";
    dealer.cards.appendChild(backcard); //aggiungo una carta coperta al dealer solo per estetica
}

async function waitForInfo(type="info",amount=0){
    if(is_running)
        return;
    is_running=true;
    if(game_over!==""){
        showMessage(game_over);
        return;
    }
    let url = "/php/scripts/get_game_info.php?";
    let param="";
    if(type==="info"){
        param="info";
        let info_value = (received_game_info) ? "" : "=all";
        param = param + info_value;
    }else if(turn === my_id){
        param= "move="+ type;
        if(amount>0)
            param = param + "&amount=" + amount;
    }

    let risposta;
    try{
        risposta = await requestJson(url + param); //dovrei fare try/catch
    }catch(error){
        alert(error.message);
        showMessage("ERRORE: " + error.message);
        is_running=false;
        return;
    }
    if(risposta.successo==true){
        if(risposta.moves.length>0){
            clearTimer();
            moves_buffer.push(...risposta.moves);
        }
        if('over' in risposta && risposta.over == true){ 
            await playMoves(); //in teoria ricevo over solo se sono io a fare il turno dove finisce la partita no?
            hideEverything();
            showMessage("PARTITA TERMINATA");
            return;
        }
        if(!received_game_info){
            let start=document.getElementById('start_button');
            entry.hidden=true;
            if(start)
                start.remove();

            inizializzaStrutturePartita(risposta);
            await playMoves();
            if( risposta.turn == my_id && time_to_wait <20) 
                playGiveCardsAnimation(0);
            else
                await playGiveCardsAnimation(400);
            received_game_info=true;
        }
        await playMoves();

        if(received_game_info) //cambio il turno solo se ho ricevuto le informazioni per farlo
            changeTurn(risposta.turn);
    
    }else if(risposta.error != "hold"){ //se è hold non faccio niente
        let is_empty= (risposta.error=="STANZA VUOTA");
        const start_button = document.getElementById('start_button');
        start_button.hidden= !is_empty;
        showMessage(risposta.error,(is_empty)?2:0); //se l errore è stanza vuota mostro il messaggio per 2 secondi
        is_running=false;
        return;
    }
    is_running=false;
    if(risposta.turn !== my_id || !received_game_info) //se è il mio turno interrompo le richieste
        setTimeout(waitForInfo,WAITING_TIME);
}

async function inizializzaStanzaGioco(){
    entry = document.getElementById('entry_message');
    timer = document.getElementById('timer');

    const d=document.getElementById('dealer');
    dealer={
        div: d,
        cards:d.getElementsByClassName('card_container')[0]
    };

    document.getElementById('fast').addEventListener('click',changeSpeed);
    document.getElementById('check').addEventListener('click',checkButton);
    document.getElementById('bet').addEventListener('click',betButton);
    document.getElementById('betamount').addEventListener('input',checkBetAmount);
    document.getElementById('fold').addEventListener('click',foldButton);

    if(isCreator){
        const start_button = document.getElementById('start_button');
        start_button.addEventListener('click',sendStartSignal);
    }else
        waitForInfo();
}
document.addEventListener('DOMContentLoaded',inizializzaStanzaGioco);
