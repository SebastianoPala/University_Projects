function [R,Qb]=fast_qr(A,b)
    [ri,co]=size(A);
    if ri>co
        n=co;
    else
        n=ri-1;
    end
    k=1;
    Qb=b;
    for i=1:n
        %[A(k:end,i:end),Qb(k:end,:)]=fast_hh(A(k:end,i:end),Qb(k:end,:));
        [A(k:end,i:end),Qb(k:end,:)]=no_for_hh(A(k:end,i:end),Qb(k:end,:));
        k = k+1;
    end
    R=A;
end