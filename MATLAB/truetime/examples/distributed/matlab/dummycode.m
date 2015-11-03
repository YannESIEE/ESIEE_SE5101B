function [exectime, data] = dummycode(seg, data)

switch seg,
  
 case 1, 
  exectime = 0.004;
 case 2,
  exectime = -1;
end
