function [exectime, data] = hdlcode(seg, data)

switch seg,
    
 case 1,
  task = ttInvokingTask; % Returns task that got an overrun.
  ttKillJob(task); % Enforce hard deadlines by 
                   % terminating job at deadline.

  exectime = -1;
end
