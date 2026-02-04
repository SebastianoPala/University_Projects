function o= valuta_lj(x,j,v)
    x=[x[1:j-1],x[j+1:end]];
    x_den= prod(x(j+1)-x); % j parte da 0
    l=length(v);
    o=zeros(l,1);
    for i=1:l
        o(i)=prod((v(i)-x)/x_den);
    end
end 