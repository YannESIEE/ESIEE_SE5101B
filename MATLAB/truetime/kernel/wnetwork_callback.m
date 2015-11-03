function wnetwork_callback

currentValue = get_param(gcb,'UserData');

val = get_param(gcb, 'Maskvalues');

if isempty(currentValue)
  currentValue = val{1};
  set_param(gcb,'UserData',currentValue);
end

if strcmp(currentValue, val{1})
  return
end

% 
currentValue = val{1};

if strcmp(val{1}, '802.11b (WLAN)') % 802.11
  set_param(gcb, 'rate', '800000', 'minsize', '272', 'transpower', '20', 'threshold', '-48', 'acktimeout', '0.00004', 'retrylimit', '5');
elseif strcmp(val{1}, '802.15.4 (ZigBee)') % 802.15.4
  set_param(gcb, 'rate', '250000', 'minsize', '248', 'transpower', '-3', 'threshold', '-48', 'acktimeout', '0.000864', 'retrylimit', '3');
else
  disp(val{1})
end

set_param(gcb,'UserData',currentValue);
