function [exectime, data] = rcvcode(seg, data)

disp(['Node ' num2str(data.nbr) ' received ping at ' num2str(ttCurrentTime)])

exectime = -1;
