function det = laplace_det(A)
    det= 0;
    if size(A,1)~=size(A,2)
        fprintf("matrice non quadrata\n");
        return;
    end
    dim=size(A,1);
    if dim==1
        det = A;
        return;
    end
    if dim==2
        det= A(1,1)*A(2,2) - A(1,2)*A(2,1);
        return;
    end
    meno=1;
    for i= 1:dim
        det= det+ meno*A(1,i)*laplace_det(submatr(A,1,i));
        meno= -meno;
    end
end