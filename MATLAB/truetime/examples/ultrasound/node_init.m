function node_init(nbr)

% Initialize TrueTime kernel
ttInitKernel(0, 0, 'prioFP'); % nbrOfInputs, nbrOfOutputs, fixed priority

if nbr==1
  % Create sensor task
  ttCreatePeriodicTask('ping_task', 1.0, 1, 1, 'pingcode');
end

data.nbr = nbr;

% Initialize network
ttCreateInterruptHandler('nw_handler', 1, 'rcvcode', data);
ttInitNetwork(nbr, 'nw_handler');
