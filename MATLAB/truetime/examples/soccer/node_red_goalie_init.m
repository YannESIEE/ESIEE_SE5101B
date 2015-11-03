function node_red_goalie_init(argument)

% Master, provides reference trajectories

global xPos yPos Kx Ky

nodeID = argument;

% Initialize TrueTime kernel
ttInitKernel(2, 2, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

data.nodeID = nodeID;
data.u1 = 0;
data.u2 = 0;
data.K1 = Kx(nodeID);
data.K2 = Ky(nodeID);
data.xref = xPos(nodeID);
data.yref = yPos(nodeID);

ttCreateEvent('redGoaliePacket');
ttCreateTask('CtrlredGoalie', 0.05, 1, 'goalieCtrl', data);
ttCreateJob('CtrlredGoalie');

% Initialize network
ttCreateInterruptHandler('nw_handler', 1, 'redMsgRcvMaster', data);
ttInitNetwork(nodeID, 'nw_handler'); 
