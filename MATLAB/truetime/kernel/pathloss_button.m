function pathloss_button()

val = get_param(gcb, 'use_pathloss_function');


temp=get_param(gcb, 'MaskEnables');

if strcmp(val, 'on')
  temp{8}='off';
  temp{10}='on';
  set_param(gcb, 'MaskEnables', temp);
else
  temp{8}='on';
  temp{10}='off';
  set_param(gcb, 'MaskEnables', temp);
end
