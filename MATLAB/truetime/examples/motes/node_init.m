function node_init(args)

% Global variables used for the rendering
global xPos yPos pingPeriod visitPoints

% Name of the node
nodeID = args;

% Initialize TrueTime kernel
ttInitKernel(2, 2, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

data.nodeID = nodeID;
data.x = xPos(nodeID);
data.y = yPos(nodeID);
data.visit = visitPoints;    % Local data list of locations to visit
data.ref = []; % Empty matrix means that we have not yet chosen destination
data.done = [data.x; data.y];% Local data list of visited locations
offset = pingPeriod*rand;    % An offset smaller than the period
ttCreatePeriodicTask('Task', offset, pingPeriod, 10, 'code', data);

% Initialize network
data_hdl.nodeID = nodeID;
ttCreateInterruptHandler('nw_handler', nodeID, 'msgRcv', data_hdl);
ttInitNetwork(nodeID, 'nw_handler');






