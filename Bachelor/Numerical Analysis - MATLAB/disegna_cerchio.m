function disegna_cerchio(centro,raggio)
    cir=linspace(0,2*pi,1000);
    plot(cos(cir)*raggio + centro(1),sin(cir)*raggio +centro(2));
end