function X= cramer_sol(A,b)
    X=zeros(size(b,1),size(b,2));
    if size(A,1) ~= size(b,1)
        fprintf("matr e vett incompatibile\n");
        return;
    end
    detA=det(A);
    for j=1:size(b,2)
        for i=1:size(b,1)
            X(i,j)= det([A(:,1:i-1),b(:,j),A(:,i+1:end)])/detA;
        end
    end
end