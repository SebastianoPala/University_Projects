function interp_lagrange(x,y)
    %close all;
    x_val = min(x):0.01:max(x);
    l=length(x_val);
    y_val=zeros(1,l);
    for i=1:l
        for j=1:length(x)
            y_val(i)=y_val(i)+ y(j)*base_lagr(x,x_val(i),j);
        end
    end
    hold on;
    plot(x,y,"xr");
    plot(x_val,y_val,"-g");
    hold off;
end

function y = base_lagr(x,xj,k)
    y=1;
    if x(k)==xj
        return;
    end
    l=length(x);
    for i=1:l
        if i ~= k
            y=y* ((xj-x(i))/(x(k)-x(i)));
            if y == 0
                return;
            end
        end
    end
end