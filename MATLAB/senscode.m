function [exectime, data] = senscode(seg, data)

switch seg,
 case 1, 
  th = ttAnalogIn(data.thChanel);
  d  = ttAnalogIn(data.dChanel);
  data.y=[th; d];
  exectime = 0.00005;
 case 2,
  ttSendMsg(3,data.y,length(data.y)); % Send message (80 bits) to node 3 (controller)
  exectime = 0.0004;
 case 3,
  exectime = -1; % finished
end
