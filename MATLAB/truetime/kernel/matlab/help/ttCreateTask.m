% TTCREATETASK   Create a TrueTime task.  
%
%  Usage: ttCreateTask(name, deadline, priority, codeFcn) 
%         ttCreateTask(name, deadline, priority, codeFcn, data) 
%
%  Inputs:
%    name      Name of the task, must be unique.
%    deadline  The relative deadline of the task.
%    priority  The priority of the task. Should be a positive number,
%              where a small number represents a high priority.
%              Only relevant for fixed-priority scheduling.
%    codeFcn   The code function of the task. Should be a string
%              with the name of the corresponding m-file.
%    data      An arbitrary data structure used to store
%              task-specific data. Optional argument.
%       
% See also TTCREATEPERIODICTASK, TTCREATEJOB, TTSETX
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
