function H=householder_mat(v)
    
    nv=norm(v,2);
    vt=v;
    if vt(1)>=0
        vt(1)=vt(1)+nv;
    else
        vt(1)=vt(1)-nv;
    end
    nv=norm(vt,2);
    H=eye(size(v,1))-((2*vt*vt')/(nv*nv));
    
end