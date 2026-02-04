function [Lf,U,c]=my_gauss(A,b)
    G=[A b];
    Lf=eye(size(A,1));
    for i= 1:size(A,1)-1;
        L=eye(size(A,1));
        L(i+1:end,i)=-G(i+1:end,i).'/G(i,i);
        G= L*G;
        Lf=Lf*L;
        
    end
    Lf=Lf - eye(size(A,1));
    Lf=-Lf;
    Lf = Lf + eye (size(A,1));
    U=G(:,1:size(A,2));
    c=G(:,size(A,2)+1:end);
end