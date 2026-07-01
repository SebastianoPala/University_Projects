function Hj=my_jacobi(A)
    Hj=(-A+diag(diag(A)))./diag(A);
end