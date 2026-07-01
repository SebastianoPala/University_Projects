function [Q,R]=my_qr(A)
    n=min(size(A));
    if size(A,1) <= size(A,2)
        n=n-1;
    end
    Q=eye(size(A,1));
    for i=1:n
        H=householder_mat(A(i:end,i));
        H=blkdiag(eye(i-1),H);
        A=H*A;
        %Q= Q*H';
        Q= H*Q;
    end
    Q=Q';
    R=A;
end