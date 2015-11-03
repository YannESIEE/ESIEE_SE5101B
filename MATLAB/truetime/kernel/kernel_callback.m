function kernel_callback

currentValue = get_param(gcb,'UserData');

val = get_param(gcb, 'battery');

if isempty(currentValue)
  currentValue = val;
  set_param(gcb,'UserData',currentValue);
end

% Do nothing if nothing has changed
if strcmp(currentValue, val)
  return
end

currentValue = val;

% Disable the library link so that we can do some dirty rebuilding
if ( strcmp(get_param(gcb, 'LinkStatus'), 'resolved') )
  set_param(gcb, 'LinkStatus', 'inactive');
end

blocket = sprintf('%s/E', gcb);
position = get_param(blocket, 'Position');
delete_block(blocket);

if strcmp(val, 'on')
  add_block('built-in/Inport', blocket, 'Position', position)
else
  add_block('built-in/Constant', blocket, 'Position', position)
end

set_param(gcb,'UserData',currentValue);
