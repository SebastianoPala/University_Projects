function a=vand_newt(x,y)
    l=length(x);
    V=zeros(l,l);
    V(:,1)=ones(l,1);
    b=1;
    ntn=1;
    for i=2:l
        b=b+1;
        ntn=1;
        for j=2:b
            ntn=ntn*(x(i)-x(j-1));
            V(i,j)= ntn;
        end
    end
    a=V\(y.');
end