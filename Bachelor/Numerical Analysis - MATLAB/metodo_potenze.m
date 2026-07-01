function [z,lamvec]= metodo_potenze(A,z0,maxit)
    zold=z0;
    for i=1:maxit
        zold=zold/norm(zold);
        z=A*zold;
        lamvec(i)=zold'*z;
        zold=z;
    end
end