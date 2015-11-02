function [exectime, data] = ctrlcode(seg, data)

switch seg,
 case 1, 
  data.y = ttGetMsg;         % Obtain sensor value
  %th=y(1,1);
  %d=y(2,1);
  %data.y=[th d]';
  data.x=data.Adc*data.x+data.Bdc*data.y;
  data.u=-data.Cdc*data.x;
  exectime = 0.0005;
 case 2,
  ttSendMsg(2, data.u, length(data.u));    % Send 80 bits to node 2 (actuator)
  exectime = -1; % finished
end
