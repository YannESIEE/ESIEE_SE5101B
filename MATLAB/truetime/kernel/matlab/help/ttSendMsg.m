% TTSENDMSG   Send a message over a network.
%             This function is used both for wired and wireless networks.
%
%  Usage: ttSendMsg(receiver, data, length)
%         ttSendMsg(receiver, data, length, priority)
%         ttSendMsg([network receiver], data, length)
%         ttSendMsg([network receiver], data, length, priority)
%
%  Inputs:
%    receiver  The number of the receiving node (a number between
%              1 and the number of nodes). It is allowed to send 
%              messages to oneself. Specify receiver number 0 to 
%              broadcast a message to all nodes in the network. 
%    data      The contents of the message (arbitrary data).
%    length    The length of the message, in bits. Determines 
%              the time it will take to transmit the message.
%              NOTE: Changed from bytes to bits in TrueTime 1.4
%    priority  The priority of the message (relevant only for
%              CSMA/AMP networks). If not specified, the priority 
%              will be given by the number of the sending node,
%              i.e., messages sent from node 1 will have the
%              highest priority by default.
%    network   The network interface on which the message should 
%              be sent. The default network number is 1. 
%       
% See also TTINITNETWORK, TTGETMSG
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
