function synch_init

% TrueTime task synchronization using monitors.
%
% A cascaded controller for a ball-and-beam process (3rd order
% system) is implemented using separate tasks for the two loops in
% the cascade. The output from the outer controller is used as
% input to the inner controller. This variable is a shared
% resource, and mutual exclusion is achieved by a TrueTime monitor.

% Initialize TrueTime kernel
ttInitKernel(3, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

outer_prio = 7.0;
outer_period = 0.020;
outer_offset = 0;

inner_prio = 8.0;
inner_period = 0.020;
inner_offset = 0;

% Create data structs for the two controller tasks
outerData.pid = newpid(-0.45, 100, 0.85, 10, outer_period, 1);
outerData.u = 0;

innerData.pid = newpid(10, 0, 0, 10, inner_period, 0);
innerData.u = 0;
innerData.ref = 0;
innerData.phi = 0;

% Create periodic controller tasks
ttCreatePeriodicTask('Outer', outer_offset, outer_period, outer_prio, 'outercode', outerData);
ttCreatePeriodicTask('Inner', inner_offset, inner_period, inner_prio, 'innercode', innerData);

% Global variable for communication between the controller tasks
global outerU
outerU = 0;

% Create monitor for mutual exclusion when accessing outerU
ttCreateMonitor('mutex', 1);
