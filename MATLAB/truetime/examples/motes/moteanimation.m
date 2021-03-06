function [sys,x0,str,ts]=moteanimation(t,x,u,flag,params,xInit,yInit);

%ANIMATION S-function for animating the motion of the motes.

switch flag,
 case 0,
  [sys,x0,str,ts] = mdlInitializeSizes(params,xInit,yInit);
 case 2,
  sys = mdlUpdate(t,x,u);
 case 3,
  sys = mdlOutputs(t,x,u);
 case 9,
  sys = mdlTerminate(t,x,u);
 otherwise
  error(['Unhandled flag = ',num2str(flag)]);
end

%=============================================================================
% mdlInitializeSizes
%=============================================================================

function [sys,x0,str,ts]=mdlInitializeSizes(params,xInit,yInit)

% Global variables used for the rendering
global transmitPower receiverThreshold pathloss motehdl transrad motetext visitPoints vpoint

% Read parameters
numMotes = params(1);
ts = params(2);
xmax = params(3);
ymax = params(4);

% Initialize the figure for use with this simulation

figure(1),clf;
tit=title('Coordination example');
set(tit,'FontSize', 10);
set(gcf,'Renderer','OpenGL');
set(gcf,'Position',[20 350 700 700])
%grid on
set(gca,'Xlim',[-xmax xmax],'Ylim',[-ymax ymax],'nextplot','add');
set(gca,'XTickLabel',[]);
set(gca,'YTickLabel',[]);
set(gca, 'XTick', -xmax:1:xmax,'XColor', [0 0 0]);
set(gca, 'YTick', -ymax:1:ymax,'YColor', [0 0 0]);

moteradius = 0.3;

powlin = 0.001*10^(transmitPower/10);       %dbm->Watt
threslin = 0.001*10^(receiverThreshold/10); %dbm->Watt
%how far does the signal reach
reach = (powlin/threslin)^(1/pathloss) - 1;

grad = 0:10:360;
% x and y coordinates to paint a mote
X = moteradius*cos(grad*pi/180); 
Y = moteradius*sin(grad*pi/180); 

for k = 1:numMotes
  moteX = X'+xInit(k);
  moteY = Y'+yInit(k);
  moteColour = [1 1 0];
  %the mote
  motehdl{k} = patch(moteX, moteY, moteColour);
  %the transmission radius
  radiusColor = [0 0.6 0.7];
  transrad(k) = patch(reach*(1/moteradius)*X'+xInit(k),reach*(1/moteradius)*Y'+yInit(k), radiusColor);
  set(transrad(k),'FaceAlpha', 0.5); % genomskinlig
  motetext(k) = text(xInit(k)-1.8*moteradius, yInit(k)+1.8*moteradius, num2str(k));
end

%Paint the visit points
for k = 1:size(visitPoints,2)
  shapeX = visitPoints(1, k) + [-1  -1   -0.1 -0.1 0.1 0.1 1   1 ];
  shapeY = visitPoints(2, k) + [-0.1 0.1  0.1  1   1   0.1 0.1 -0.1];
  vpoint(k) = patch(shapeX, shapeY, [1 0 0]);
end

sizes = simsizes;
sizes.NumContStates  = 0;
sizes.NumDiscStates  = 0;          
sizes.NumOutputs     = 0;
sizes.NumInputs      = 2*numMotes; % x and y plus cart velocity 
sizes.DirFeedthrough = 0;
sizes.NumSampleTimes = 1;   

sys = simsizes(sizes);

x0  = [];
str = [];
ts  = [ts 0]; % Hybrid block


%=============================================================================
% mdlOutputs
%=============================================================================

function [sys]=mdlOutputs(t,x,u)

sys = [];

%=============================================================================
% mdlUpdate
%=============================================================================

function [sys]=mdlUpdate(t,x,u)

global transmitPower receiverThreshold pathloss motehdl transrad motetext
persistent first_time;

numMotes = length(motehdl);

moteradius = 0.3;
grad = 0:10:360;
X = moteradius*cos(grad*pi/180); 
Y = moteradius*sin(grad*pi/180); 

powlin = 0.001*10^(transmitPower/10);
threslin = 0.001*10^(receiverThreshold/10);
reach = (powlin/threslin)^(1/pathloss) - 1;

%update graphics
for k=1:numMotes
  moteX = X'+u(k);
  moteY = Y'+u(k+numMotes);
  radX = (1/moteradius)*reach*X' + u(k);
  radY = (1/moteradius)*reach*Y' + u(k+numMotes);
  %transmission radius
  set(transrad(k),'XData', radX, 'YData', radY);
  %the mote
  set(motehdl{k},'XData', moteX, 'YData', moteY);
  %text
  set(motetext(k), 'Position', [u(k)-1.8*moteradius u(k+numMotes)+1.8*moteradius]);
end

% Try to synchronize the animation with the computer time
if isempty(first_time),
  first_time = clock;
end
while etime(clock, first_time)*2<t,
  %disp('waiting')
end


%disp(t)


drawnow;

sys=[];


%=============================================================================
% mdlTerminate
% Perform any end of simulation tasks.
%=============================================================================

function sys=mdlTerminate(t,x,u)

sys = [];