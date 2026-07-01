function o= valuta_lagrange(x,f,v)
    l=length(v);
    o=zeros(l,1);
    y=arrayfun(f,x);
    for i=1:l
        o=o+valuta_lj(x,i-1,v)*y(i);
    end
end