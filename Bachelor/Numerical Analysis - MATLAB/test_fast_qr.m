n=1500;
%testing and plotting various QR implementations
tempi=zeros(4,n);
for i=1:n
    fprintf("iterazione %d\n",i);
 A=randn(i);
 b=randn(i);

 tic;
 [Q,R]=qr(A);
 bt=Q'*b;
 tempi(1,i) = toc;
 tic;
 [Q,R]=my_qr(A);
 bt=Q'*b;
 tempi(2,i) = toc;
 tic;
 [Rf,bf]=fast_qr(A,b);
 tempi(3,i) = toc;
 tic;
 [Rn,bn]=no_for_qr(A,b);
 tempi(4,i) = toc;
end
close all;
hold on; 
plot(1:n, tempi(1,:), "-r", 'DisplayName', 'qr');
plot(1:n, tempi(2,:), "-b", 'DisplayName', 'my qr');
plot(1:n, tempi(3,:), "-k", 'DisplayName', 'fast qr');
plot(1:n, tempi(4,:), "-m", 'DisplayName', 'no for qr');

legend('show'); 
grid on;         
xlabel('Dimensione problema (n)');
ylabel('Tempo di esecuzione (s)');
title('Confronto tempi di esecuzione algoritmi QR');
hold off;