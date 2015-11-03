function [pid, u] = calculateOutput(pid, r, y)

% Calculate PID output.

pid.e = r - y;
pid.D = pid.Td/(pid.N*pid.h+pid.Td)*pid.D + pid.N*pid.K*pid.Td/(pid.N*pid.h+pid.Td)*(pid.e - pid.eold);

u = pid.K*(r - y) + pid.I + pid.D;