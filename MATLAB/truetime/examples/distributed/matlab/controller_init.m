function controller_init(arg)

% Distributed control system: controller node
%
% Receives messages from the sensor node, computes control signal
% and sends it to the actuator node. Also contains a high-priority
% disturbing task.

% Initialize TrueTime kernel
ttInitKernel(1, 0, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

% Controller parameters
h = 0.010;
N = 100000;
Td = 0.035;
K = 1.5;

% Create task data (local memory)
data.u = 0.0;
data.K = K; 
data.ad = Td/(N*h+Td);
data.bd = N*K*Td/(N*h+Td);
data.Dold = 0.0;
data.yold = 0.0;

% Create controller task
deadline = h;
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
