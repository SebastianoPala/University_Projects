function x=inf_solve(A,b)
    A=tril(A);
    x=zeros(size(A,1),1);
    for i=1:size(A,1)
        x(i)= (b(i)- (A(i,1:i-1)*x(1:i-1)))/A(i,i);
       
    end
    
end