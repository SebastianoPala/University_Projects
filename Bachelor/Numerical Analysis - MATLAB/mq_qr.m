function [x,r] = mq_qr(A,b)
    [Q,R]=my_qr(A);
    c=Q'*b;
    n=min(size(A,1),size(A,2));
    x=sup_solve(R(1:n,1:n),c(1:n));
    fprintf("Norma residuo: %f , norma ottima: %f\n",norm(R(1:n,1:n)*x-c(1:n))+norm(c(n+1:end)),norm(c(n+1:end)));
    r=norm(R(1:n,1:n)*x-c(1:n))+norm(c(n+1:end));
end