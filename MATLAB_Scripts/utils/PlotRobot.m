function h= PlotRobot(Xk,RobotType,d,LineObj)

h_plot= LineObj;

x_c=  Xk(1);
y_c=  Xk(2);
th= Xk(3);

C= [x_c; y_c];

if nargin<3
    h= figure();
end

% P1     \
% : d/2   \
% Xk - d - P3
% :       /
% P2     /


if RobotType == 1
    color1=[1 0 0];
    color2=[1 0 0];
elseif RobotType == 2 
    color1=[0 0 1];
    color2=[0 0 1];
else
    color1=[1 0 1];
    color2=[0 1 0];
end

M= [cos(th) -sin(th); sin(th) cos(th)];

P1= [ 0 ; +d/3 ];
P2= [ 0 ; -d/3 ];
P3= [ d ;   0  ];

P1= M*P1 + C;
P2= M*P2 + C;
P3= M*P3 + C;

set(h_plot,'XData',[P1(1) P3(1) P2(1) P1(1)],...
           'YData',[P1(2) P3(2) P2(2) P1(2)],'color',color1,'LineWidth',2);
     
drawnow update

end