'use strict';
const initialMode = window.appData;

const all_text=[["Non hai un account?","Hai gia un account?"],["Crealo qui","Accedi"]]
const modes=["Signup","Login"];

function controlloInput(){
    const submit = document.getElementById('submit');
    if(submit.value === "Login"){
        submit.disabled=false;
        return;
    }
    let ok=true;

    const usr = document.getElementById('username');
    if(!usr.value.match(/^[A-Za-z0-9]{4,18}$/)){
        usr.setCustomValidity("L'username inserito non va bene");
        ok=false;
    }else
        usr.setCustomValidity("");

    const pswd = document.getElementById('password');
    if(!pswd.value.match(/^(?=.*\d)(?=.*[^a-zA-Z0-9\s]).{8,20}$/)){
        pswd.setCustomValidity("La password inserita non va bene");
        ok=false;
    }else
        pswd.setCustomValidity("");

    const r_pswd = document.getElementById('r_password');
    if(r_pswd.required == true && pswd.value !== r_pswd.value){
        r_pswd.setCustomValidity("Le due password non corrispondono");
        ok=false;
    }else
        r_pswd.setCustomValidity("");
    
    submit.disabled = ok ? false : true;
}

function cambiaModalita(){
    const form = document.getElementById('form');
    const r_passw=document.getElementById('r_password');
    r_passw.required = !r_passw.required;
    
    const mess = document.getElementById('mess');
    if(mess)
        mess.remove();

    form.querySelectorAll('input:not([type=submit])').forEach(elem=>{elem.value="";
                                                                elem.setCustomValidity("");});

    form.querySelectorAll('.hide').forEach(elem => {elem.hidden = !elem.hidden;});
    
    const children = document.getElementById('change_container').children;
    let i=0;
    for(let child of children){
        all_text[i].unshift(child.textContent);
        child.textContent=all_text[i].pop();
        i++;
    }

    const submit = document.getElementById('submit');
    modes.unshift(submit.value);
    submit.value=modes.pop();
    if(submit.value==="Login")
        submit.disabled=false;
    else
        submit.disabled=true;
}

function inizializzaLogin(){
    const submit = document.getElementById('submit');
    submit.value=modes.pop();
    
    if(initialMode === "Signup") 
        cambiaModalita();

    const change_button = document.getElementById('change');
    change_button.addEventListener('click',cambiaModalita);

    const inputs=document.querySelectorAll('input');
    for(let i of inputs){
        if(i.type==="submit")
            continue;
        i.addEventListener('input',controlloInput);
    }
}
document.addEventListener('DOMContentLoaded',inizializzaLogin);