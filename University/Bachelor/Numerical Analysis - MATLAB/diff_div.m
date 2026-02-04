function d=diff_div(x,y)
    l=length(x);
    d=zeros(l,l);
    d(:,1)=y;
    b=1;
    for i=2:l
        b=b+1;
        for j=b:l
            d(j,i)=(d(j,i-1)-d(i-1,i-1))/(x(j)-x(b-1));
        end
    end
    d;
    d=diag(d);
end