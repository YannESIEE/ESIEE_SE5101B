function threeservos_init

% Task scheduling and control.
%
% This example extends the simple PID control example (located in
% $DIR/examples/simple_pid) to the case of three PID-tasks running
% concurrently on the same CPU controlling three different servo
% systems. The effect of the scheduling policy on the global control
% performance is demonstrated.

% Initialize TrueTime kernel
ttInitKernel(6, 3, 'prioRM'); % nbrOfInputs, nbrOfOutputs, rate-monotonic

% Task parameters
periods = [0.006 0.005 0.004];
names = {'pid_task1', 'pid_task2', 'pid_task3'};
rChans = [1 3 5];
yChans = [2 4 6];
uChans = [1 2 3];

% Create the three tasks
for i = 1:3
  data.K = 0.96;
  data.Ti = 0.12;
  data.Td = 0.049;
  data.beta = 0.5;
  data.N = 10;
  data.h = periods(i);
  data.u = 0;
  data.Iold = 0;
  data.Dold = 0;
  data.yold = 0;
  data.rChan = rChans(i);
  data.yChan = yChans(i);
  data.uChan = uChans(i);

  % Offset=0 and prio=1 for all tasks
  ttCreatePeriodicTask(names{i}, 0, periods(i), 1, 'pidcode', data);
end