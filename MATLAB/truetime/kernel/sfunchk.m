function r = sfunchk(name)

e = exist(name, 'file');

if e == 2
  r = 'ttkernelMATLAB';
elseif e == 3
  % mex-file
  r = name;
else
  error(['No m-function or mex-function named ''' name ''' found!'])
end
