function interp_monomi(x,y)
    %close all;
    l=length(x);
    if l ~= length(y)
        fprintf("dimensioni errate\n");
        return;
    end
    hold on;
    plot(x,y,"xb");
    V=x.'*ones(1,l);
    
    for i=1:l
        V(:,i) = V(:,i).^(i-1);
    end
    
    %non controllo invertibilita
    a=V\y.';
    x_vals=min(x):0.01:max(x);
    y_vals=zeros(1,length(x_vals));
    for i=1:length(x_vals)
        y_vals(i)=approx(x_vals(i),a);
    end
    plot(x_vals,y_vals,"-r");
    hold off;
    
end

function y= approx(x,a)
    l=length(a);
    y=0;
    for i=1:l
        y= y+ (x^(i-1))*a(i);
    end
end