function getmsg_callback

currentValue = get_param(gcb,'UserData');

mask_values = get_param(gcb,'MaskValues');

if isempty(currentValue)
  currentValue = mask_values;
  set_param(gcb,'UserData',mask_values);
end

index = 3;

% Do nothing if nothing has changed
if strcmp(currentValue{index}, mask_values{index})
  return
end

% Disable the library link so that we can do some dirty rebuilding
if ( strcmp(get_param(gcb, 'LinkStatus'), 'resolved') )
  set_param(gcb, 'LinkStatus', 'inactive');
end

blocks = {'trigger'};
  
blocket = sprintf('%s/%s', gcb, blocks{1});
position = get_param(blocket, 'Position');
delete_block(blocket)

if strcmp(mask_values{index}, 'on')
  add_block('built-in/Outport', blocket, 'Position', position)
else
  add_block('built-in/Terminator', blocket, 'Position', position)
end

set_param(gcb,'UserData',mask_values);
