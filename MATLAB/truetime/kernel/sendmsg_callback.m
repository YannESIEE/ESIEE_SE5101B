function sendmsg_callback

currentValue = get_param(gcb,'UserData');

mask_values = get_param(gcb,'MaskValues');
mask_enables = get_param(gcb,'MaskEnables');
%val = get_param(gcb, 'network_input');

if isempty(currentValue)
  currentValue = mask_values;
  set_param(gcb,'UserData',mask_values);
end

% Do nothing if nothing has changed
if strcmp(currentValue, mask_values)
  return
end

% Disable the library link so that we can do some dirty rebuilding
if ( strcmp(get_param(gcb, 'LinkStatus'), 'resolved') )
  set_param(gcb, 'LinkStatus', 'inactive');
end

blocks = {'network', 'sender', 'receiver', 'data', 'length', 'priority'};

%index = 2;

for index = 1:length(blocks),
  
  if strcmp(currentValue{index*2}, mask_values{index*2})
    continue;
  end
  
  blocket = sprintf('%s/%s', gcb, blocks{index});
  position = get_param(blocket, 'Position');
  delete_block(blocket)

  if strcmp(mask_values{index*2}, 'on')
    add_block('built-in/Inport', blocket, 'Position', position)
    mask_enables{index*2-1} = 'off';
  else
    add_block('built-in/Constant', blocket, 'Position', position)
    set_param(blocket, 'Value', blocks{index})
    mask_enables{index*2-1} = 'on';
  end

  set_param(gcb,'MaskEnables', mask_enables)
  
end

TriggerType = get_param(gcb, 'TriggerType');
blocket = sprintf('%s/%s', gcb, 'Trigger');
set_param(blocket, 'TriggerType', TriggerType);

set_param(gcb,'UserData',mask_values);
