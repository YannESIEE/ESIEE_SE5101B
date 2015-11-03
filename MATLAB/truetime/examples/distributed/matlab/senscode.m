function [exectime, data] = senscode(seg, data)

switch seg,
 case 1, 
  data.y = ttAnalogIn(1);
  exectime = 0.00005;
 case 2,
  ttSendMsg(3, data.y, 80); % Send message (80 bits) to node 3 (controller)
  exectime = 0.0004;
 case 3,
  exectime = -1; % finished
end
