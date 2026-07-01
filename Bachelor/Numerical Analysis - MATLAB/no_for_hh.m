function [vnew,bnew]=no_for_hh(v,b)
    vf=v(:,1);
    l=size(v,2);
    vf(1)=vf(1)+sign(vf(1)+(vf(1)==0))*norm(vf);
    beta=2/(vf'*vf);
    g=[v,b]; 
    gres=zeros(size(g));
    
    gres=g-beta*vf*(vf'*g);
    
    vnew=gres(:,1:l);
    bnew=gres(:,l+1:end);
end