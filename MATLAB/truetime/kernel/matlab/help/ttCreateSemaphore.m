% TTCREATESEMAPHORE   Create a simple, counting TrueTime semaphore
%
%  Usage: ttCreateSemaphore(name, initval)
%         ttCreateSemaphore(name, initval, maxval)
%
%  Inputs:
%    name      Name of the semaphore, must be unique.
%    initval   The initial value of the semaphore.
%    maxval    The maximum value of the semaphore (default MAX_INT)
%       
% See also TTTAKE, TTGIVE
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
