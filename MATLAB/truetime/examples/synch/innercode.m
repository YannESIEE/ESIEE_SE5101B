function [exectime, data] = innercode(seg, data)

% Code function for the inner controller task in the
% cascaded structure.

% Global variable for communication between the 
% controller tasks, here used as reference signal.
global outerU

switch seg,
  
 case 1,
  ttEnterMonitor('mutex');
  exectime = 0;
  
 case 2,
  data.phi = ttAnalogIn(2); % read beam angle
  data.ref = outerU;        % reference signal from outer controller
  exectime = 0.0005;
  
 case 3,
  ttExitMonitor('mutex'); 
  exectime = 0;
  
 case 4,
  [data.pid, data.u] = calculateOutput(data.pid, data.ref, data.phi);
  exectime = 0.002;
  
 case 5,
  ttAnalogOut(1, data.u); % output control signal
  data.pid = updateState(data.pid);
  exectime = -1; % finished

end

