function d=diff_div_ott(x,y)
    l=length(x);
    d=y.';
    b=1;
    for i=2:l
        for j=b+1:l
            d(j)=(d(j)-d(b))/(x(j)-x(b));
        end
        b=b+1;
    end
end