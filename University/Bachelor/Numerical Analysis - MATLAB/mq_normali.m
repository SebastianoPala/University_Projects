function [x,r]=mq_normali(A,b)
    x=sys_solve(A'*A,A.'*b);
    r= norm(A*x -b);
    fprintf("norma residuo: %f\n",r);
end