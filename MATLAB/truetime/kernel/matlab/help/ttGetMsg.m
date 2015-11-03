% TTGETMSG   Get a message that has been received over a network.
%            This function is used both for wired and wireless networks.
%
%  Usage: [msg, signalPower] = ttGetMsg
%         [msg, signalPower] = ttGetMsg(network)
%
%  Inputs:
%    network     The network interface from which the message should 
%                be received. The default network number is 1. 
%    signalPower The value of the received signal power
%                corresponding to this message. Only used in the
%                wireless network.
%       
% See also TTINITNETWORK, TTSENDMSG
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
