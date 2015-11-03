% TTSETNETWORKPARAMETER   Set a specific network parameter on a per
%                         node basis.
%
%  Usage: ttSetNetworkParameter(parameter, value)
%         ttSetNetworkParameter(networkNbr, parameter, value)
%
%  Inputs:
%    parameter  The name of the parameter to be changed.
%               Can be any of:
%
%               'transmitpower'
%               'predelay'
%               'postdelay'
%
%    value      The new value of the parameter.
%    networkNbr The network interface on which the paramter should
%               be changed. Defaults to 1.
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
