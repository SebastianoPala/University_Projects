function B= submatr(A,i,j)
    B= [A(1:i-1,1:j-1),A(1:i-1,j+1:end);A(i+1:end,1:j-1),A(i+1:end,j+1:end)];
end