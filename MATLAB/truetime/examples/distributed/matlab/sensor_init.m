function sensor_init

% Distributed control system: sensor node
%
% Samples the plant periodically and sends the samples to the 
% controller node.

% Initialize TrueTime kernel
ttInitKernel(1, 0, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Create sensor task
data.y = 0;
offset = 0;
period = 0.010;
prio = 1;
ttCreatePeriodicTask('sens_task', offset, period, prio, 'senscode', data);

% Initialize network
ttCreateInterruptHandler('nw_handler', prio, 'msgRcvSensor');
ttInitNetwork(4, 'nw_handler'); % node #4 in the network
