s = tf('s');

k = 186.067;
u = 0.50327;
y = 0.59859;
g = 9.8;
l = 0.3;

Gj = l*(-s*k/g)/(s^2+u*s+k)+1/s

step(G)
sisotool(G)

Ti = 0.24;
Td = 0.06;
Kp = 19.88;

%C = Kp*(1+1/(Ti*s)+Td*s);

Ki = 25;
Kd = 0.13;
Kp = 15;

C = Kp*(1+Ki/s+s*Kd);

%sisotool(G,C)