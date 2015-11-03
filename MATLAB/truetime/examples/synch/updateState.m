function pid = updateState(pid)

% Update PID state.

if (pid.intOn ~= 0) 
  pid.I = pid.I + (pid.K*pid.h/pid.Ti)*(pid.e);
end
pid.eold = pid.e;
