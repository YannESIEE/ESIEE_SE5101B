function network_callback

m = get_param(gcb,'MaskValues');

nnodes = str2num(m{3});
if nnodes < 1
  error('Number of nodes must be greater than zero')
end

nwNbr = str2num(m{2});
if nnodes < 1
  error('Network number must be greater than zero')
end

switch m{1},
 case 'CSMA/CD (Ethernet)',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on','off','off','off','off','off','off'})

 case 'CSMA/CD-AMP (CAN)',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on','off','off','off','off','off','off'})
  
 case 'Round Robin',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on','off','off','off','off','off','off'})

 case 'FDMA',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on', 'on', 'off','off','off','off','off'})
  bandwidths = str2num(m{7});
  if length(bandwidths) < nnodes
    bandwidths = [bandwidths zeros(1,nnodes-length(bandwidths))];
    m{7} = mat2str(bandwidths);
    set_param(gcb,'MaskValues',m)
  elseif length(bandwidths) > nnodes
    bandwidths = bandwidths(1:nnodes);
    m{7} = mat2str(bandwidths);
    set_param(gcb,'MaskValues',m)
  end
  
  if (sum(bandwidths)>1)
    error('The bandwidths must sum to at most 1')
  end
  
 case 'TDMA',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on','off','on','on','off','off','off'})
  
 case 'Switched Ethernet',
  set_param(gcb,'MaskEnables',{'on','on','on','on','on','on','off','off','off','on','on','on'})

end