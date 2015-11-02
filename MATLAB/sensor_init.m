function sensor_init

% Distributed control system: sensor node
%
% Samples the plant periodically and sends the samples to the 
% controller node.

% Initialize TrueTime kernel
ttInitKernel(2, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Create sensor task
data.th = 0;
data.d=0;
data.y=[0;0];
data.u=0;
data.thChanel=1;
data.dChanel=2;
offset = 0;
period = 0.010;
deadline=3*period;
prio = 3;
ttCreatePeriodicTask('sens_task', offset, period, prio, 'senscode', data);

ttCreateTask('act_task', deadline, 4, 'actcode', data);

% Initialize network
ttCreateInterruptHandler('nw_handler', 2, 'msgRcvSensor');
ttInitNetwork(2, 'nw_handler'); % node #4 in the network