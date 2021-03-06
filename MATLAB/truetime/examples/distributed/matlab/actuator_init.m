function actuator_init

% Distributed control system: actuator node
%
% Receives messages from the controller and actuates 
% the plant.

% Initialize TrueTime kernel
ttInitKernel(0, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Create actuator task
deadline = 100;
prio = 1;
ttCreateTask('act_task', deadline, prio, 'actcode');

% Initialize network
ttCreateInterruptHandler('nw_handler', prio, 'msgRcvActuator');
ttInitNetwork(2, 'nw_handler'); % node #2 in the network
