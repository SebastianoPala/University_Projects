function det = gauss_det(A)
    [L,U,c] = my_gauss(A,zeros(size(A,1),1));
    det= prod(diag(U));
end