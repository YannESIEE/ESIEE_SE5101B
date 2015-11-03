% TTCREATEINTERRUPTHANDLER   Create a TrueTime interrupt handler.
%
%  Usage: ttCreateInterruptHandler(name, priority, codeFcn) 
%         ttCreateInterruptHandler(name, priority, codeFcn, data)
%
%  Inputs:
%    name      Name of the hamdler, must be unique.
%    priority  The priority of the handler. Should be a positive number,
%              where a small number represents a high priority.
%    codeFcn   The code function of the handler. Should be a string
%              with the name of the corresponding m-file.
%    data      An arbitrary data structure used to store
%              handler-specific data. Optional argument.
%       
% See also TTCREATETIMER, TTCREATEEXTERNALTRIGGER, TTINITNETWORK,
%          TTATTACHDLHANDLER, TTATTACHWCETHANDLER
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
