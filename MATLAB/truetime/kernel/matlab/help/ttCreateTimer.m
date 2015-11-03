% TTCREATETIMER   Create a one-shot timer and associate an interrupt
%                 handler with the timer.
%
%  Usage: ttCreateTimer(timername, time, handlername)
%
%  Inputs:
%    timername    Name of the timer, must be unique.
%    time         The time when the timer is set to expire.
%    handlername  Name of the interrupt handler to be associated 
%                 with the timer.
%       
% See also TTCREATEINTERRUPTHANDLER, TTCREATEPERIODICTIMER, 
%          TTREMOVETIMER
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
