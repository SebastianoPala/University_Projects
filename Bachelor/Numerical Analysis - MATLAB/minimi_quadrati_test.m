clear
m = 10;
n = 5;
A = zeros(m,n);
for j = 1:n
A = A + randn(m,1)*randn(1,n)*10^(-2*(j-1));
end
b = randn(m, 1);

[xn,rn]=mq_normali(A,b)

[xq,nq]=mq_qr(A,b)