function controller_init(arg)

% Distributed control system: controller node
%
% Receives messages from the sensor node, computes control signal
% and sends it to the actuator node. Also contains a high-priority
% disturbing task.

% Initialize TrueTime kernel
Synthese;

ttInitKernel(0, 0, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Controller parameters
T_ech = 0.010;

data.Adc=Adc;
data.Bdc=Bdc;
data.Cdc=Cdc;
dada.Ddc=Ddc;

data.x=[0 0 0 0]';
data.y=[0 0]';
data.u=0;

% Create controller task
deadline = T_ech;
prio = 2;
ttCreateTask('pid_task', deadline, prio, 'ctrlcode', data);
% Optional disturbance task
if arg > 0
  offset = 0.0002;
  period = 0.007;
  prio = 1;
  ttCreatePeriodicTask('dummy', offset, period, prio, 'dummycode');
end

% Initialize network
ttCreateInterruptHandler('nw_handler', prio, 'msgRcvCtrl');
ttInitNetwork(3, 'nw_handler'); % node #3 in the network
