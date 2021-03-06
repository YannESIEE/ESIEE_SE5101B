function node_init(args)

% Wireless Ad-hoc Routing Using AODV.
%
% This example shows an implementation of Ad-hoc On-Demand Distance
% Vector (AODV) routing in TrueTime. A scenario involving seven 
% nodes is used, all initialized by this initialization script.
%
% Node 1 sends data periodically to node 7 with period 0.5.
% The initial route that is established is 1 -> 3 -> 5 -> 7.
% At time t=3, node 5 breaks the route by moving away. The route 
% is repaired by node 6, creating the route 1 -> 2 -> 5 -> 6 -> 7.

nodeID = args;

% Initialize TrueTime kernel
ttInitKernel(2, 2, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

ttCreateMailbox('AODVSendBox', 20); % Data messages to AODV layer
ttCreateMailbox('AODVRcvBox', 20);  % Data messages from AODV layer

% AODV task to send application messages
% initiates route discovery if necessary
dataAODVsnd.nodeID = nodeID;
dataAODVsnd.RREQID = 0;   
dataAODVsnd.buffer = {};  % message buffer
dataAODVsnd.buflen = zeros(1,7); 
dataAODVsnd.emptyBuffer = 0;
dataAODVsnd.bufInd = 0;
prio = 5;
ttCreateTask('AODVSendTask', 1, prio, 'AODVsendcode', dataAODVsnd);

% Task to process incoming messages
% higher prio than AODVSend to allow faster routes to be
% obtained while sending buffered data
dataAODVrcv.nodeID = nodeID;
dataAODVrcv.cache = {};    % cache for processed RREQs
dataAODVrcv.cnt = 0;
dataAODVrcv.RERRlist = {}; % list of RERRs to propagate
prio = 1;
ttCreateTask('AODVRcvTask', 1, prio, 'AODVrcvcode', dataAODVrcv);

% Periodic task to send HELLO messages and
% discover/handle lost connections
datahello.nodeID = nodeID;
datahello.lastRREQ = -100;
datahello.nbors = {};
datahello.cnt = 0;
datahello.RERRlist = {}; % list of RERRs to send
offset = 0.1*rand(1); % to avoid collisions between HELLO msgs
prio = 3;
HELLO_INTERVAL = 1;
ttCreatePeriodicTask('HelloTask', offset, HELLO_INTERVAL, prio, 'hellocode', datahello);

% Timer handler
ttCreateInterruptHandler('timer_handler', 2, 'timercode');
% Task to handle expiry of routing table entries
data_exp.nodeID = nodeID;
data_exp.expTimer = 0; % flag to indicate timer set
prio = 4;
ttCreateTask('TimerTask', 1, prio, 'expcode', data_exp);


% Application tasks
% -----------------
% Node 1 periodically sends data to Node 7
% Jobs of RcvTask in node 7 created from AODV layer
% Node 5 starts moving at time 3  to break the route
% Node 6 starts moving at time 10 to re-establish the route
prio = 10;
period = 0.5;
if (nodeID==1)
  datasend.nodeID = nodeID;
  ttCreatePeriodicTask('SendTask', 0, period, prio, 'APPLsendcode', datasend);
end
if (nodeID==5)
  ttCreatePeriodicTask('MoveTask', 0, period, prio, 'APPLmovecode5');
end
if (nodeID==6)
  ttCreatePeriodicTask('MoveTask', 0, period, prio, 'APPLmovecode6');
end
if (nodeID==7)
  datarcv.nodeID = nodeID;
  ttCreateTask('RcvTask', 1, 10, 'APPLrcvcode', datarcv);
end
  
% Initialize network
data_nwhdl.nodeID = nodeID;
ttCreateInterruptHandler('nw_handler', 2, 'nwrcvcode', data_nwhdl);
ttInitNetwork(nodeID, 'nw_handler'); 
