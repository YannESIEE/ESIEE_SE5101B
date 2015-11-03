% TTCREATEEVENT   Create a TrueTime event.
%
%  Usage: ttCreateEvent(eventname)
%         ttCreateEvent(eventname, monitorname)
%
%  Inputs:
%    eventname    Name of the event, must be unique.
%    monitorname  Name of a monitor to which the event is to be
%                 associated as a condition variable. Optional
%                 argument, if not specified the event will be free.
%       
% See also TTWAIT, TTNOTIFY, TTNOTIFYALL
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
