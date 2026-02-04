function x =iter_gs(A,b,k)
    Hgs=crea_matr_gs(A);
    c=tril(A)\b;
    xold=b;
    for i=1:k
        x=Hgs*xold + c;
        xold=x;
        fprintf("iterazione %d, norma: %f\n",i,norm(A*x-b,2));
    end
end