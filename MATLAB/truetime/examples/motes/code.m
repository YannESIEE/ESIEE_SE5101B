function [exectime, data] = code(seg, data)

switch seg,
 case 1,
  % check what the other nodes are doing
  msgin = ttGetMsg;
  while ~isempty(msgin)
    if ~isempty(msgin.done)
      % update the coordinates visited by the group
      data.done = union(data.done', msgin.done', 'rows')';
      data.visit = setdiff(data.visit', data.done', 'rows')';
    end
    
    % Are we heading in the same direction as someone else?
    % If so, and if we have lower "priority", then change direction
    if ~isempty(data.ref) & ~isempty(data.visit) & ...
	  msgin.heading == data.ref & msgin.priority>=data.priority
      disp(['node ' num2str(data.nodeID) ...
	    ' is heading in the same direction as another node and'])
      disp(['  our priority: ' num2str(data.priority) ' other priority: ' ...
	    num2str(msgin.priority) '==> we change direction.'])
      index = floor((length(data.visit)-1)*rand + 1);
      data.ref = data.visit(:,index);
    end    
    msgin = ttGetMsg;
  end
    
  % Are we heading to a place that has already been visited?
  if ~isempty(data.ref) & ~isempty(data.visit) & ~ ...
	isempty(intersect(data.ref', data.done','rows'))
    disp(['node ' num2str(data.nodeID) ' is heading to a visited place'])
    data.ref = []; %taken care of below
  end
    
  % First time or heading to an already visited place
  % Choose a random location in the list
  if isempty(data.ref)
    disp(['node ' num2str(data.nodeID) ' is changing direction'])
    index = floor((length(data.visit)-1)*rand + 1);
    data.ref = data.visit(:,index);
    data.priority = rand;
    data.visit = data.visit(:, 1:end~=index);
  end
  
  % Have we arrived at out destination?
  distance_left = sqrt((data.ref(1)-data.x)^2+(data.ref(2)-data.y)^2);
  if ~isempty(data.visit) & distance_left<0.3
    disp(['node ' num2str(data.nodeID) ' is visiting ' ...
	  'x=' num2str(data.ref(1)') ' y=' num2str(data.ref(2)')])

    % Colour it
    global vpoint visitPoints;
    index = intersect(find(visitPoints(1,:)==data.ref(1)), ...
		      find(visitPoints(2,:)==data.ref(2)));
    set(vpoint(index),'FaceColor', [0 1 0]);
    
    data.done(:,end+1) = data.ref;
    index = floor((length(data.visit)-1)*rand + 1);
    data.ref = data.visit(:,index);
    data.priority = rand;
    data.visit = data.visit(:, 1:end~=index);
  end
  
  exectime = 0.002;
 case 2,
  % Read our position
  data.x = ttAnalogIn(1);
  data.y = ttAnalogIn(2);

  % Compute the error and the control signal
  e = [data.ref(1)-data.x, data.ref(2)-data.y];
  u = 1.*e; %P-controller
  
  % Limit the control signal
  umin = -5;
  umax = 5;
  u = min(umax, u);
  u = max(umin, u);
    
  % Output the control signal
  ttAnalogOut(1, u(1));
  ttAnalogOut(2, u(2));
    
  % Tell the others where we are heading
  msgout.sender = data.nodeID;
  msgout.heading = [data.ref(1); data.ref(2)];
  msgout.priority = data.priority; % How much do we want to visit the point?
  msgout.done = data.done;
  ttSendMsg(0, msgout, 160);
  exectime = 0.002;
 case 3,
  exectime = -1;
end
