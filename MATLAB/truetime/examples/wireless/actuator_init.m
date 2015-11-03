function actuator_init

% Distributed control system: actuator node
%
% Receives messages from the controller and actuates 
% the plant.

% Initialize TrueTime kernel
ttInitKernel(1, 1, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority
ttSetKernelParameter('energyconsumption', 0.0100); %10 mW

% Create mailboxes
ttCreateMailbox('control_signal', 10)
ttCreateMailbox('power_ping', 10)
ttCreateMailbox('power_response', 10)

% Create sensor task
data.y = 0;
offset = 0;
period = 0.010;
prio = 1;
ttCreatePeriodicTask('sens_task', offset, period, prio, 'senscode', data);

% Create actuator task
deadline = 100;
prio = 2;
ttCreateTask('act_task', deadline, prio, 'actcode');

% Create power controller task
offset = 2.07;
period = 0.025;
prio = 3;
power_data.transmitPower = 20;
power_data.name = 1;     % We are node number 1 in the network
power_data.receiver = 2; % We are communicating with node 2
power_data.haverun = 0;  % We have not run yet
ttCreatePeriodicTask('power_controller_task', offset, period, prio, 'powctrlcode', power_data);

% Create power response task
deadline = 100;
prio = 4;
ttCreateTask('power_response_task', deadline, prio, 'powrespcode');

% Initialize network
ttCreateInterruptHandler('nw_handler', prio, 'msgRcvActuator');
ttInitNetwork(1, 'nw_handler'); % node #1 in the network