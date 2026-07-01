function [z,lamvec]= potenze_inverse(A,z0,maxit)
    zold=z0;
    [L,U]=my_gauss(A,[]);
    for i=1:maxit
        zold=zold/norm(zold);
        y=inf_solve(L,zold);
        z=sup_solve(U,y);
        lamvec(i)=1/(zold'*z);
        zold=z;
    end
end