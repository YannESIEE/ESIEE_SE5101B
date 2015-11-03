env = getenv('TTKERNEL');
if isempty(env)
  error('Environment variable TTKERNEL not defined - please quit MATLAB and set this variable first.')
end

ccd = pwd;

try 

  cd(env)

  disp('Compiling TrueTime kernel block...');
  mex ttkernelMATLAB.cpp % Compile kernel block
  disp('...done.')
  disp('');
  disp('Compiling TrueTime network block...');
  mex ttnetwork.cpp      % Compile network block
  disp('...done.')
  disp('Compiling TrueTime wireless network block...');
  mex ttwnetwork.cpp      % Compile wireless network block
  disp('...done.')
  
  cd matlab
  
  disp('Compiling TrueTime MEX-functions...');
  disp('');
  
  compileall % Will compile all TrueTime MEX-functions
  
  disp('');
  disp('TrueTime compiled successfully!')
  
catch
  
  disp('Compilation using MEX failed! (Run ''mex -setup'' to configure your compiler)')

end
  
cd(ccd)
