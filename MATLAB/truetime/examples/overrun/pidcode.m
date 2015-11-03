function [exectime, data] = pidcode(seg, data)

switch seg,
    
 case 1,
  r = ttAnalogIn(data.rChan); % Read reference
  y = ttAnalogIn(data.yChan); % Read process output
  data = pidcalc(data, r, y); % Calculate PID action
  % Stochastic execution time causing overruns
  exectime = 0.005 + 0.002*rand;

 case 2,
  ttAnalogOut(data.uChan, data.u); % Output control signal
  exectime = -1;
end
