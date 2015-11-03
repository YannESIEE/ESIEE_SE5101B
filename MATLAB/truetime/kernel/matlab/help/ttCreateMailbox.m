% TTCREATEMAILBOX   Create a mailbox for inter-task communication.
%
%  Usage: ttCreateMailbox(mailboxname)
%  Usage: ttCreateMailbox(mailboxname, maxsize)
%
%  Inputs:
%    mailboxname  Name of the mailbox, must be unique.
%    maxsize      The size of the buffer associated with the
%                 mailbox (default is INT_MAX) 
%       
% See also TTTRYPOST, TTTRYFETCH, TTPOST, TTFETCH, TTRETRIEVE
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
