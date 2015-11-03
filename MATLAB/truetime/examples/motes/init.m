global transmitPower receiverThreshold pathloss errorCodingThreshold nbrOfMotes pingPeriod xPos yPos visitPoints

transmitPower = -20;     %dbm
receiverThreshold = -48; %dbm
pathloss = 3.5;
errorCodingThreshold = 0.03;
nbrOfMotes = 3;
pingPeriod = 1;

% Initial locations of the three motes
xPos = [-18 -18 -18];
yPos = [-18 -18 -18];

% Coordinates that should be visited by the group
visitPoints = [1  5 10 15 18 -5  -15 -15 19;
	       2 15 18  7  1  10  15  0 -19];