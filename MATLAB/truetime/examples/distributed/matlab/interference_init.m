function interference_init

% Distributed control system: interference node
%
% Generates disturbing network traffic.

% Initialize TrueTime kernel
ttInitKernel(0, 0, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Create sender task
offset = 0;
period = 0.001;
prio = 1;
ttCreatePeriodicTask('interf_task', offset, period, prio, 'interfcode');

% Initialize network
ttCreateInterruptHandler('nw_handler', prio, 'msgRcvInterf');
ttInitNetwork(1, 'nw_handler'); % node #1 in the network

