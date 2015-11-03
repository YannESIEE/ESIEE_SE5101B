env = getenv('TTKERNEL');
if isempty(env)
  error(['Environment variable TTKERNEL not defined - please quit MATLAB and set this variable first. See Section 2 of the reference manual'])
end

addpath([getenv('TTKERNEL') '/matlab/help']) 
addpath([getenv('TTKERNEL') '/matlab']) 
