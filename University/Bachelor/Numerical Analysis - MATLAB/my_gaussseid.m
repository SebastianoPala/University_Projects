function Hgs=my_gaussseid(A)
    Hgs= -triu(A,1);
    Hgs(:,2:end)=tril(A)\Hgs(:,2:end);
end