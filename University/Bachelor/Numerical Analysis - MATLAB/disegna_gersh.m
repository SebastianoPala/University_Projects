function disegna_gersh(A)
    close all;
    if size(A,1) ~= size(A,2)
        fprintf("matrice non quadrata gang\n");
        return;
    end
    cir=linspace(0,2*pi,1000);
    hold on;
    centri=diag(A);
    diaglessA= abs(A-diag(centri));
    raggi= diaglessA*ones(size(A,1),1);
    raggi_trasp= diaglessA.'*ones(size(A,1),1);
    autov=eig(A);
    for i=1:size(A,1)
        disegna_cerchio_ott(centri(i),raggi_trasp(i),cir,"g");
        disegna_cerchio_ott(centri(i),raggi(i),cir,"b");
        plot(real(autov(i)),imag(autov(i)),"xr");
    end
    alpha(0.05)
    hold off;
end

function disegna_cerchio_ott(centro,raggio,cir,style)
    plot(cos(cir)*raggio + real(centro),sin(cir)*raggio +imag(centro),style);
    patch(cos(cir)*raggio + real(centro),sin(cir)*raggio +imag(centro),style);
end