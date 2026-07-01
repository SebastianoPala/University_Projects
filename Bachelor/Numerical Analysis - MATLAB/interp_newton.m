function interp_newton(x,y)
    %close all
    x_val=min(x):0.01:max(x);
    l=length(x_val);
    y_val=zeros(1,l);
    %d=vand_newt(x,y); 
    %d=diff_div(x,y);
    d=diff_div_ott(x,y);
    for i=1:l
        b_ntn=1;
        for j=1:length(x)
            y_val(i)=y_val(i)+d(j)*b_ntn;
            b_ntn=b_ntn*(x_val(i)-x(j));
        end
    end
    hold on;
    plot(x,y,"xg");
    plot(x_val,y_val,"-b");
    hold off;
end