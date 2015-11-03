% TTCREATELOG   Create a log structure for task attributes.
%
%  Usage: ttCreateLog(taskname, logtype, variable, size)
%
%  Inputs:
%    taskname  Name of the task for which the log is created.
%    logtype   The log type, should be any of:
%                 
%              1  - Response time log
%              2  - Release latency log, time between arrival
%                   and release of each task job
%              3  - Start latency log, time between arrival
%                   and start of execution for each task job
%              4  - Execution time log
%              5  - Context switch log, logs the time instants
%                   when the context is restored if context 
%                   switching is being simulated
%              6  - User-controlled log, written to using
%                   ttLogNow, ttLogStart, and ttLogStop
%
%    variable  The name of the variable in MATLAB workspace to
%              which the log will be written after the simulation.
%    size      The maximum number of elements in the log.
%       
% See also TTLOGNOW, TTLOGSTART, TTLOGSTOP
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
