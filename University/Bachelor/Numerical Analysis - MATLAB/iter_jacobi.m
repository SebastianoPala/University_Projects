function x=iter_jacobi(A,b,k)
    Hj=crea_matr_jacobi(A);
    c=b./diag(A);
    xold=b;
    for i=1:k
        x=Hj*xold + c;
        xold=x;
        fprintf("iterazione %d, norma: %f\n",i,norm(A*x-b,2));
    end
end