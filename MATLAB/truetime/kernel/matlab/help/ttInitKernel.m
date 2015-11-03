% TTINITKERNEL   Initialize the TrueTime kernel.
%
%  Usage: ttInitKernel(nbrInp, nbrOutp, prioFcn) 
%         ttInitKernel(nbrInp, nbrOutp, prioFcn, cs_oh)
%
%  Inputs:
%    nbrInp   Number of A/D channels of the kernel block.
%    nbrOutp  Number of D/A channels of the kernel block.
%    prioFcn  Scheduling policy, should be any of:
%                 
%             'prioFP'  - Fixed-priority scheduling
%             'prioRM'  - Rate-monotonic scheduling
%             'prioDM'  - Deadline-monotonic scheduling
%             'prioEDF' - Earliest-deadline-first scheduling
%
%    cs_oh    Overhead associated with a full context switch. 
%             Optional argument, taken as zero if not specified.
%
% See also TTCREATETASK, TTCREATEPERIODICTASK
%
%       TrueTime, Version 1.5
%       Copyright (c) 2007
%       Martin Ohlin, Dan Henriksson and Anton Cervin
%       Department of Automatic Control LTH
%       Lund University, Sweden
