function [L,U,P,c]=my_lu(A,b)
    G=[A b];
    [ri,co]=size(G);
    P=eye(ri);
    p=1:ri;
    L=P;
    if ri<=co
        n= ri-1;
    else
        n=co;
    end
    for i=1:n;
        [~,id]=max(abs(G(i:end,i)));
        if id >1
            id=id+i-1;
            temp=1:ri;
            temp(i)=id;
            temp(id)=i;
            G=G(temp,:);
            L=L-eye(ri);
            L= L(temp,:) + eye(ri);
            p=p(temp);
        end
        Lt=eye(ri);
        if G(i,i) == 0
            error("determinante = 0, impossibile proseguire\n");
        end
        Lt(i+1:end,i)=-G(i+1:end,i)/G(i,i);
        G=Lt*G;
        L(i+1:end,i)= - Lt(i+1:end,i);
    end
    P=P(p,:);
    U=G(:,1:size(A,2));
    c=G(:,size(A,2)+1:end);
    
end