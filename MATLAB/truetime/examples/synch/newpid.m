function pid = newpid(K, Ti, Td, N, h, intOn)

% This function initializes the data structure of a PID controller.

pid.K = K;
pid.Ti = Ti;
pid.Td = Td;
pid.N = N;
pid.h = h;
pid.intOn = intOn;
pid.e = 0;
pid.eold = 0;
pid.D = 0;
pid.I = 0;