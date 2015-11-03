function [exectime, data] = pingcode(seg, data)

switch seg,
 case 1, 
  disp(['Node 1 sending ping at ' num2str(ttCurrentTime)])
  ttUltrasoundPing;
  exectime = -1; % finished
end
