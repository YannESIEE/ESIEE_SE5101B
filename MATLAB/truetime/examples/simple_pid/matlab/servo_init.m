function servo_init(mode)

% PID-control of a DC servo process.
%
% This example shows four ways to implement a periodic controller
% activity in TrueTime. The task implements a standard
% PID-controller to control a DC-servo process (2nd order system). 

% Initialize TrueTime kernel
ttInitKernel(2, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Task attributes
period = 0.006;
deadline = period;
offset = 0.0; % start of first task job
prio = 1;

% Create task data (local memory)
data.K = 0.96;
data.Ti = 0.12;
data.Td = 0.049;
data.beta = 0.5;
data.N = 10;
data.h = period;
data.u = 0;
data.Iold = 0;
data.Dold = 0;
data.yold = 0;
data.rChan = 1;
data.yChan = 2;
data.uChan = 1;

switch mode,
 case 1, 
% IMPLEMENTATION 1: using the built-in support for periodic tasks

 ttCreatePeriodicTask('pid_task', offset, period, prio, 'pidcode1', data);

 case 2, 
% IMPLEMENTATION 2: calling Simulink block within code function

 data2.u = 0; % Only the control signal needs to be stored between
               % segments. Controller states are stored internally by
               % TrueTime.
 ttCreatePeriodicTask('pid_task', offset, period, prio, 'pidcode2', data2);

 case 3, 
% IMPLEMENTATION 3: sleepUntil and loop back

 data.t = 0;
 ttCreateTask('pid_task', deadline, prio, 'pidcode3', data);
 ttCreateJob('pid_task');

 case 4, 
% IMPLEMENTATION 4: sampling in timer handler, triggers task job

 hdl_data.yChan = 2;
 ttCreateInterruptHandler('timer_handler', prio, 'samplercode', hdl_data);
 ttCreatePeriodicTimer('timer', offset, period, 'timer_handler');
 ttCreateMailbox('Samples', 10);
 ttCreateTask('pid_task', deadline, prio, 'pidcode4', data);

end