.global cifra_cesare
# funzione che prende in ingresso il puntatore al messaggio da cifrare in %rdi, e un numero 
# compreso tra 1-25 in %rsi. %rdx contiene la modalità. cifra il messaggio secondo il cifrario di cesare 
cifra_cesare:
    #salvataggio registri
    push %rcx

    mov %rsi,%rcx #ora il numero si trova in %cl
    cld # azzero il direction flag
    mov %rdi,%rsi


    mov $0,%rax #preparo il ritorno

    #la chiave deve essere compresa tra 1-25
    cmp $0,%cl
    je bad_cesare_loop
    cmp $25,%cl
    ja bad_cesare_loop

    mov $-26,%ah #è il numero che verrà usato per fare mod 26
    cmp $'D',%dl #se devo decifrare cambio il segno alla chiave
    jne ahead_cesare
    neg %cl
    neg %ah

    ahead_cesare:
    mov $200,%ch #impongo un numero massimo di iterazioni per evitare accessi in memoria errati

    cifra_cesare_loop:
        dec %ch
        jz fine_cesare_loop

        lodsb
        cmp $0,%al #verifico se è il carattere di fine stringa
        je fine_cesare_loop
        
        mov %al,%dh
        and $0xDF,%dh
        cmp $'A',%dh
        jb skip_cesare_loop
        cmp $'Z',%dh
        ja skip_cesare_loop #salto i caratteri speciali

        mov %al,%dh #salvo %al
        add %cl,%al # effettuo la cifratura
        and $0xDF,%dh
        add %cl,%dh # verifico se %al + chiave è ancora una lettera
        cmp $'Z',%dh
        jbe skip_cesare_loop # in questo caso non si è verificato overflow
        cmp $'A',%dh
        jae skip_cesare_loop

        add %ah,%al# in questo caso si, dunque faccio %al mod 26 

        skip_cesare_loop:
        stosb
    jmp cifra_cesare_loop

    fine_cesare_loop:
    mov $1,%rax  #se si arriva qua, l'operazione è andata a buon fine
    bad_cesare_loop: # se si salta qua dai controlli, %rax conterra' 0
    pop %rcx
    ret

###############################

#sottoprogramma che calcola l'inverso di un numero in modulo n usando l'algoritmo di euclide esteso iterativamente. prende in ingresso il numero a che si vuole invertire
#in %al, e il numero m su cui fare il modulo in %ah. non controlla se i numeri a,m sono coprimi
.global inverso_mod_n
.set old_r,-2
.set r,-4
.set old_c,-6
.set c,-8
.set quoz,-10 
.set mod,-12
inverso_mod_n:
    push %rbp
    mov %rsp,%rbp
    sub $16,%rsp

    push %rdx
    push %rcx
    push %rbx
    #resto vecchio = %ah
    #resto attuale = %al
    movzbw %ah,%bx
    mov %bx,old_r(%rbp)
    mov %bx,mod(%rbp)
    movzbw %al,%ax
    mov %ax,r(%rbp)
    movw $0,old_c(%rbp)
    movw $1,c(%rbp)
    inverso_loop:
        mov r(%rbp),%bx
        cmp $0,%bx
        je fine_inverso_loop
        
        xor %dx,%dx
        movw old_r(%rbp),%ax
        div %bx #resto nuovo in %dx, quoziente in %ax
        mov %ax,quoz(%rbp)
        mov c(%rbp),%bx
        push %dx
        mul %bx
        pop %dx
        mov old_c(%rbp),%bx
        sub %ax,%bx # bx=coeff nuovo
        mov r(%rbp),%ax
        mov %ax,old_r(%rbp)
        mov %dx,r(%rbp)
        mov c(%rbp),%ax
        mov %ax,old_c(%rbp)
        mov %bx, c(%rbp)

    jmp inverso_loop

    fine_inverso_loop:
    xor %rax,%rax
    mov old_r(%rbp),%ax
    cmp $1,%ax
    jne no_inverso
    mov old_c(%rbp),%ax
    cmp $0,%ax
    jge no_neg_inverso
    mov mod(%rbp),%bx
    add %bx,%ax
    no_inverso:
    no_neg_inverso:
    pop %rbx
    pop %rcx
    pop %rdx
    leave
    ret


.global cifra_affine
# %rdi contiene il puntatore al messaggio, %rsi contiene a e %rdx contiene b. %rcx contiene il tipo (se cifratura o decifratura). il messaggio viene cifrato secondo
# la formula y= (a*x +b) mod 26 , con y lettera del messaggio cifrato. per avere cifratura univoca 
# a DEVE essere coprimo con 26. 
cifra_affine:
    mov %rsi, %rax
    mov %al, %dh # %dh conterrà a, mentre %dl conterrà b
    cld
    push %rbx

    mov %rdi,%rsi

    mov $0,%rax

    # b deve essere compreso tra 0-25
    cmp $25,%dl
    ja bad_cifra_affine
    # a deve essere coprimo con 26, dunque dispari e diverso da 13
    cmp $0,%dh
    je bad_cifra_affine
    cmp $26,%dh
    jae bad_cifra_affine
    cmp $13,%dh
    je bad_cifra_affine
    mov %dh,%bl
    and $0x01,%bl
    jz bad_cifra_affine # il numero è pari

    cmp $'D',%cl
    jne ahead_affine
    mov $26,%ah
    mov %dh,%al
    call inverso_mod_n
    mov %al,%dh

    ahead_affine:
    mov $200, %ch #limito le iterazioni

    cifra_affine_loop:
        dec %ch
        jz fine_cifra_affine

        lodsb
        cmp $0,%al
        je fine_cifra_affine # carattere di fine stringa

        mov %al,%bl
        and $0xDF,%bl
        cmp $'A',%bl
        jb skip_affine_loop
        cmp $'Z',%bl
        ja skip_affine_loop #salto i caratteri speciali
        
        mov %al, %bh
        and $0xE0,%bh # salvo i 3 msb
        and $0x1F,%al # azzero i 3 msb
        dec %al # ora %al è un numero da 0 a 25
        push %dx
        cmp $'D',%cl
        je affine_decifratura
        # dunque qua si cifra, e la formula è (a*x + b) mod 26 = y
        mul %dh # a*x, con x gia in %al e risultato in %ax
        movzbw %dl,%dx
        add %dx,%ax # ora devo fare mod 26 di %ax
        mov $26,%dl
        divb %dl
        mov %ah, %al

        jmp rebuild_affine
        affine_decifratura: #la formula è (a^-1 *(y-b)) mod 26 = x
        sub %dl,%al
        cmp $0, %al
        jge no_underflow_affine
        add $26,%al
        no_underflow_affine:
        mul %dh
        mov $26, %dh
        divb %dh
        mov %ah,%al

        rebuild_affine:
        inc %al
        or %bh, %al
        pop %dx
        skip_affine_loop:
        stosb
    jmp cifra_affine_loop

    fine_cifra_affine:
    mov $1,%rax
    bad_cifra_affine:
    pop %rbx
    ret

##########################

.global cifra_completo

# funzione che prende in ingresso da %rsi un seed, e genera una permutazione per cifrare il messaggio passato
# tramite %rdi. modalità passata da %rdx
.set com_seed, -8

cifra_completo:
    push %rbp
    mov %rsp,%rbp
    sub $64,%rsp
    cld

    mov %dl,%cl #sposto la modalita per evitare di sovrascriverla con le mul
    mov $200,%ch
    movq $1,%r8 #il seed viene calcolato qua
    xor %r9,%r9
    com_seed_loop:
        dec %ch
        jz fine_seed_loop
        
        lodsb
        cmp $0,%al
        je fine_seed_loop # fine stringa
        cmp $13,%al
        je fine_seed_loop
        cmp $10,%al
        je fine_seed_loop # caratteri di ritorno carrello

        movzbq %al,%rax
        mul %r8
        mov %rax,%r8
    jmp com_seed_loop
    fine_seed_loop:
    mov %r8,coom_seed(%rbp) #non so se giusto
    # ora il seed è contenuto in %r8
    leave
    ret
