function [exectime, data] = outercode(seg, data)

% Code function for the outer controller task in the
% cascaded structure.

% Global variable for communication between the 
% controller tasks, written to by this task.
global outerU

switch seg,
  
 case 1,
  r = ttAnalogIn(1); % read reference
  x = ttAnalogIn(3); % read ball position
  [data.pid, data.u] = calculateOutput(data.pid, r, x);
  exectime = 0.002;
 
 case 2,
  ttEnterMonitor('mutex'); 
  exectime = 0;
  
 case 3,
  outerU = data.u;    % Sets the reference signal used by the inner loop
  exectime = 0.0005;
  
 case 4,
  ttExitMonitor('mutex');
  exectime = 0;
  
 case 5,
  data.pid = updateState(data.pid);
  exectime = -1; % finished

end
