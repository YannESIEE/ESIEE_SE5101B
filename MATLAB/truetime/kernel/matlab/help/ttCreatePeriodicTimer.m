% TTCREATEPERIODICTIMER   Create a periodic timer and associate
%                         an interrupt handler with the timer.
%
%  Usage: ttCreatePeriodicTimer(timername, start, period, handlername)
%
%  Inputs:
%    timername    Name of the timer, must be unique.
%    start        The time for the first expiry of the timer.
%    period       The period of the timer.
%    handlername  Name of the interrupt handler to be associated 
%                 with the timer.
%       
% See also TTCREATEINTERRUPTHANDLER, TTCREATETIMER, TTREMOVETIMER
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
