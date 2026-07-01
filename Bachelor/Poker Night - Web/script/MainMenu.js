'use strict';
let menu_window;
let {username,badAttempt} = window.appData;
const MAX_FILE_SIZE = 1*1024*1024;

async function requestJson(data){
    const response = await fetch(data);
    if(!response.ok)
        throw new Error(`Errore HTTP: ${response.status}`);
    else{
        const json = await response.json();
        return json;
    }
}

function resetWindow(text){
    const title = document.getElementById('window_son');
    let hidewindow=false;
    if(menu_window.hidden === false && title && text === title.textContent){
        hidewindow=true;
    }else{
        menu_window.innerHTML="";
        const title = document.createElement('h1');
        title.id = "window_son";
        title.textContent = text;
        menu_window.appendChild(title);
        hidewindow=false;
    }
    menu_window.hidden=hidewindow;
    return hidewindow;
}

function printServerError(errormess){
    let p = document.getElementById('errmess');
    if(!p){
        p = document.createElement('p');
        p.id="errmess";
        p.classList="error_message";
        menu_window.appendChild(p);
    }
    p.textContent=errormess;
    menu_window.hidden=false;
}

function checkFile(e){
    const file  = e.target.files[0];
    const path = document.getElementById('path');
    const preview = document.getElementById('preview');
    const submit = document.getElementById('send_file');
    path.style.color="black";
    
    function setFileError(mess){
        path.style.color="red";
        path.textContent=mess;
        e.target.value="";
        preview.src = "/Assets/layout/badimg.png";
        submit.value="RIMUOVI FOTO CORRENTE";
        return;
    }

    if(!file){
        path.textContent="Nessuna";
        preview.src = "/Assets/layout/badimg.png";
        submit.value="RIMUOVI FOTO CORRENTE";
    }else
        submit.value="INVIA";
    if (!file.type.startsWith('image/')) {
        setFileError("Carica un immagine.")
        return;
    }
    if(file.size>MAX_FILE_SIZE){
        setFileError("Dimensioni maggiori di 2MB.");
        return;
    }
    const fr = new FileReader();

    fr.onload = (e) =>{
        preview.src = e.target.result;
        path.textContent = file.name;
        submit.disabled=false;
    };
    fr.onerror = (e) =>{
        setFileError("Errore nella lettura del file.");
    };
    fr.readAsDataURL(file);
}

function creaFinestraFoto(){
    if(!resetWindow("MODIFICA FOTO")){
        const form = document.createElement('form');
        form.method="POST";
        form.enctype="multipart/form-data";
        form.classList="page_style join_menu";

        const file = document.createElement('input');
        file.addEventListener('change',checkFile);
        file.type="file";
        file.name="pfp";
        file.hidden="true";
        file.accept="image/jpeg,image/jpg,image/png";
        form.appendChild(file);
        
        const new_container = document.createElement('div');
        new_container.classList="page_style join_menu";
        form.appendChild(new_container);
        
        const preview = document.createElement('img');
        preview.id="preview";
        preview.draggable=false;
        preview.src = "/Assets/layout/badimg.png";
        preview.alt = "preview";
        new_container.appendChild(preview);

        const path_label = document.createElement('p');
        path_label.classList="preview_text";
        path_label.textContent="Immagine selezionata: (Max 2MB)";
        new_container.appendChild(path_label);

        const path_preview = document.createElement('p');
        path_preview.classList="preview_text";
        path_preview.id="path";
        path_preview.textContent="Nessuna";
        new_container.appendChild(path_preview);

        const label = document.createElement('label');
        label.id="file_button";
        label.classList="page_style";
        label.addEventListener('click',()=>file.click());
        label.textContent="SELEZIONA IMMAGINE";
        form.appendChild(label);

        const submit = document.createElement('input');
        submit.id ="send_file";
        submit.name ="change_pfp";
        submit.type="submit";
        submit.classList="menu_button darken";
        submit.value="RIMUOVI FOTO CORRENTE";
        
        form.appendChild(submit);

        menu_window.appendChild(form);
    }
}

async function creaFinestraTab(type){
    if(!resetWindow(type)){
        try{
            const result = await requestJson("/php/scripts/get_table_content.php?type=" + type);
            if(result && result.successo && result.dati){
                costruisciTabella(result.dati,type);
            }else{
                printServerError(result.dati);
            }
        }catch(error){
            printServerError(error.message) ;
        }
    }
}

async function creaFinestraStanza(){
    if(!resetWindow("CREA STANZA")){
        const form = document.createElement('form');
        form.method="POST";
        form.classList="page_style join_menu";

        const radio_container = document.createElement('div');
        radio_container.id = "radio_container";
        radio_container.classList="page_style join_container";
        form.appendChild(radio_container);

        for(let i=0;i<3;i++){
            const radio = document.createElement('input');
            radio.checked = !i ? true:false;
            radio.id = "r"+(i+1);
            radio.type = "radio";
            radio.name = "room_type";
            radio.value = !i ? "Pub" : ((i==1)?"Pri":"Sin");
            radio.required= true;
            radio_container.appendChild(radio);

            const label = document.createElement('label');
            label.textContent= !i ? "Pubblica" : ((i==1)?"Privata":"Singleplayer"); 
            label.addEventListener('click',()=>
                document.getElementById("r"+(i+1)).checked=true
            );
            radio_container.appendChild(label);
        }
        const range_container = document.createElement('div');
        range_container.id = "range_container";
        range_container.classList="page_style join_container";
        form.appendChild(range_container);

        const label = document.createElement('label');
        label.textContent= "Max Giocatori:";
        range_container.appendChild(label);

        const datalist = document.createElement('datalist');
        datalist.id = "range_list";

        const range = document.createElement('input');
        range.type="range";
        range.name="num_players";
        range.max="4";
        range.min="2";
        range.step="1";
        range.value="4";
        range.required=true;
        range.setAttribute('list','range_list');  
        
        for(let i=2;i<=4;i++){
            const option = document.createElement('option');
            option.value=`${i}`;
            option.label=`${i}`;
            datalist.appendChild(option);
        }
        
        range_container.appendChild(range);
        range_container.appendChild(datalist);

        const submit = document.createElement('input');
        submit.classList="menu_button";
        submit.type='submit';
        submit.name = 'CREA';
        submit.value="CREA STANZA";
        form.appendChild(submit);

        menu_window.appendChild(form);
    }
}

async function creaFinestraJoin(){
    if(!resetWindow("UNISCITI")){
        const menu = document.createElement('form');
        menu.method="POST";
        menu.classList = "page_style join_menu";

        const label= document.createElement('label');
        label.textContent="Inserisci il codice della stanza:";
        menu.appendChild(label);
        
        const input = document.createElement('input');
        input.type = "text";
        input.name = "ROOM_ID";
        input.required=true;
        input.placeholder="INSERIRE ID";
        input.pattern="^[A-Z1-9]{6}$";
        menu.appendChild(input);

        const submit = document.createElement('input');
        submit.type ="submit";
        submit.classList="menu_button";
        submit.name = 'JOIN';
        submit.value="Entra";
        menu.appendChild(submit);
        menu_window.appendChild(menu);
    }
}

function costruisciTabella(dati,table_type){
    if(!dati.length){
        if(table_type === "LEADERBOARD")
            printServerError("Classifica vuota");
        else
            printServerError("Nessuna stanza trovata. Creane una!");  
        return;
    }
    
    let scroll_container;
    let hidden_input;
    if(table_type === "LEADERBOARD")
        scroll_container = document.createElement('div');
    else{
        scroll_container = document.createElement('form');
        scroll_container.method="POST";

        hidden_input = document.createElement('input');
        hidden_input.type="hidden";
        hidden_input.name="ROOM_ID";
        scroll_container.appendChild(hidden_input);
    }
    scroll_container.id="scroll_container";
    
    const table = document.createElement('table');
    table.id = "menu_tabella";
    
    //costruzione header
    const header = document.createElement('tr');
    header.classList = "header";
    table.appendChild(header);

    const th = document.createElement('th');
    header.appendChild(th);
    th.textContent= table_type === "LEADERBOARD" ? "Posizione":"\\";
    Object.keys(dati[0]).forEach( chiave => {
        const th = document.createElement('th');
        header.appendChild(th);
        th.textContent=chiave;
    })
    let pos = 1;
    dati.forEach(row_info => {
        const row = document.createElement('tr');
        table.appendChild(row);

        let cell = document.createElement('td');
        row.appendChild(cell);
        if(table_type==="LEADERBOARD"){
            if(username === row_info.username)
                row.classList='isplayer'
            cell.textContent = `${pos++}`;
        }else{
            const button = document.createElement('button');
            button.dataset.roomID=row_info.ID;
            button.textContent="ENTRA";
            button.classList="menu_button";
            button.addEventListener('click',(e)=>{
                                    hidden_input.value=e.currentTarget.dataset.roomID;
                                    scroll_container.submit();
            });
            cell.appendChild(button);
        }
        Object.values(row_info).forEach(value => {
            cell = document.createElement('td');
            row.appendChild(cell);
            cell.textContent = value;
        });
    });
    scroll_container.appendChild(table);
    menu_window.appendChild(scroll_container);
}

function inizializzaMenuPrincipale(){
    menu_window = document.getElementById('window');

    document.getElementById('pfp_container').addEventListener('click',creaFinestraFoto);
    document.getElementById('lista_stanze').addEventListener('click',()=>creaFinestraTab('STANZE'));
    document.getElementById('crea_stanza').addEventListener('click',creaFinestraStanza);
    document.getElementById('join_stanza').addEventListener('click',creaFinestraJoin);
    document.getElementById('show_leaderboard').addEventListener('click',()=>creaFinestraTab('LEADERBOARD'));

    if(badAttempt !== ""){
        printServerError(badAttempt);
    }
}
document.addEventListener('DOMContentLoaded',inizializzaMenuPrincipale);
