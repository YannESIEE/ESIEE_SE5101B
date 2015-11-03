function overrun_init

% Deadline overrun handling using TrueTime overrun handlers.
% 
% Period of controller task: 6 ms
% Execution time: 5 + U(0,2) ms
% 
% Two approaches are provided. The first allows the task to
% continue into next sample (no overrun handler is attached),
% whereas the second uses a simple overrun handler that terminates
% the current job if the deadline is exceeded. 

% Initialize TrueTime kernel
ttInitKernel(2, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

rand('state', 0);

% Task attributes
period = 0.006;
offset = 0.0; % start of first task instance
prio = 1;

% Create task data (local memory)
data.K = 0.96;
data.Ti = 0.12;
data.Td = 0.049;
data.beta = 0.5;
data.N = 10;
data.h = 0.006;
data.u = 0;
data.Iold = 0;
data.Dold = 0;
data.yold = 0;
data.rChan = 1;
data.yChan = 2;
data.uChan = 1;
data.count = 0;

ttCreatePeriodicTask('pid_task', offset, period, prio, 'pidcode', data);

% Attach deadline overrun handler
%ttCreateInterruptHandler('DL_handler', prio, 'hdlcode');
%ttAttachDLHandler('pid_task', 'DL_handler');
