% TTINITNETWORK   Initialize the TrueTime network interface. Must
%                 be called once for each network the node is
%                 connected to. This function is used both for
%                 wired and wireless networks.
%
%  Usage: ttInitNetwork(nodenumber, handlername)
%         ttInitNetwork(network, nodenumber, handlername)
%
%  Inputs:
%    nodenumber   The address of the node in the network. Must be 
%                 a number between 1 and the number of nodes as 
%                 specified in the dialog of the corresponding 
%                 TrueTime network block.
%    handlername  The name of the interrupt handler that should be
%                 invoked when a message arrives over the network.
%    network      The number of the TrueTime network block.
%                 The default network number is 1.
%
% See also TTSENDMSG, TTGETMSG
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
