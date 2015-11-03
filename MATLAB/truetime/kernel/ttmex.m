function ttmex(varargin)
%
% TTMEX   Compile a TrueTime mex file
%
%    ttmex myfile.cpp   is equivalent to
%
%    mex -I$TTKERNEL myfile.cpp       on Linux
%    mex('-I%TTKERNEL%','myfile.cpp') on Windows
%
env = getenv('TTKERNEL');
if isempty(env)
  error('Environment variable TTKERNEL not defined - please quit MATLAB and set this variable first.')
end

mex(['-I' env],varargin{:})
