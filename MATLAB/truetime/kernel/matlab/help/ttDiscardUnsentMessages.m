% TTDISCARDUNSENTMESSAGES   Delete any unsent messages waiting in
%    the network interface output queue. A message that has already
%    started to be sent is not affected.
%
%  Usage: nbr = ttDiscardUnsentMessages
%         nbr = ttDiscardUnsentMessages(network)
%
%  Inputs:
%    network  The network interface from which messages should be
%             deleted (default is 1).
%  Outputs:
%    nbr      The total number of deleted messages.
%
% See also TTINITNETWORK, TTSENDMSG
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
