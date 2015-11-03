function [exectime, data] = pidcode3(seg, data)

switch seg,
    
 case 1,
  data.t = ttCurrentTime;
  exectime = 0;
 case 2,
  r = ttAnalogIn(data.rChan); % Read reference
  y = ttAnalogIn(data.yChan); % Read process output
  data = pidcalc(data, r, y); % Calculate PID action
  exectime = 0.002;
 case 3,
  ttAnalogOut(data.uChan, data.u); % Output control signal
  data.t = data.t + data.h;
  ttSleepUntil(data.t); % Sleep until next period
  exectime = 0;
 case 4,
  ttSetNextSegment(2); % loop to segment 2
  exectime = 0;
  
end
