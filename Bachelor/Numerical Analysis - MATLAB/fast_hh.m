function [vnew,bnew]=fast_hh(v,b)
    vf=v(:,1);
    l=size(v,2);
    vf(1)=vf(1)+sign(vf(1)+(vf(1)==0))*norm(vf);
    beta=2/(vf'*vf);
    g=[v,b]; 
    gres=zeros(size(g,1),size(g,2));
    for i = 1:size(g,2)
        gres(:,i)=g(:,i)-beta*(vf'*g(:,i))*vf;
    end
    vnew=gres(:,1:l);
    bnew=gres(:,l+1:end);
end