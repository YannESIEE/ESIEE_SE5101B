env = getenv('TTKERNEL');
if isempty(env)
  error('Environment variable TTKERNEL not defined - please quit MATLAB and set this variable first.')
end

disp('Compiling MEX functions...')
ttmex interference_init.cpp
disp('interference_init.cpp')
ttmex actuator_init.cpp
disp('actuator_init.cpp')
ttmex controller_init.cpp
disp('controller_init.cpp')
ttmex sensor_init.cpp
disp('sensor_init.cpp')
