'use strict';
function caricaSemi(){
    const seed_path = "/Assets/layout/";
    const seed_ext = ".png"
    function seedPath(seed){
        return seed_path + seed + seed_ext;
    }
    document.querySelectorAll('img[alt=clubs]').forEach(card => card.src = seedPath("C"));
    document.querySelectorAll('img[alt=diamonds]').forEach(card => card.src = seedPath("D"));
    document.querySelectorAll('img[alt=hearts]').forEach(card => card.src = seedPath("H"));
    document.querySelectorAll('img[alt=spades]').forEach(card => card.src = seedPath("S"));
}
document.addEventListener('DOMContentLoaded',caricaSemi);