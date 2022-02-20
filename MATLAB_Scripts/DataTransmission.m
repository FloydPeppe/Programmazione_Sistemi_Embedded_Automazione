
addpath('utils/')

clc
clear
close all


% -- Import saved parameters ---- %

tmp= load("exports/parameters.mat");
prmtrs= tmp.prmtrs;

% ---------------- Time Variables -------------------- % 

% Number of Sample for the acquisition
N_sample= 1200;

% Time step
Ts= prmtrs.TIME_STEP.val;

% Time vector
t= (0:N_sample-1)*Ts;


% ------ Function handles for basic references --------- %

% Step rference
step_ref= @(t,xo,xf) t*0 + xf;

% Ramp reference, i.e. line from x_o to x_f
ramp_ref= @(t,xo,xf) xo + (xf-xo)*(t-t(1))/(t(end)-t(1));

% Triangle reference, max_val in the center 
triangle_ref= @(t,max_val) [ ramp_ref(t(1:fix(end/2)),0,1), ramp_ref(t(fix(end/2)+1:end),1,0) ]*max_val;

% Smooth step reference (from x_o to x_f)
smooth_ref= @(t,xo,xf) xo + (t-t(end)/(2*pi)*sin(2*pi*t/t(end))).*(xf-xo)/t(end);


% ---- Trajectory definition ------ %

a=     smooth_ref(t,0,1);
theta= [smooth_ref(t(1:round(end/2)),0,3*pi), smooth_ref(t(1:round(end/2)),3*pi,0)]; 

%a=     smooth_ref(t,0,0.3); 
%theta= smooth_ref(t,0,-pi)*0;

% --- Trajectory increments ------ %

a =    [     a, ones(1,2000*0)*a(end)     ];
theta= [ theta, ones(1,2000*0)*theta(end) ];


delta_a=  diff(a);      delta_a=  [delta_a  delta_a(end)*0 ];
delta_th= diff(theta);  delta_th= [delta_th delta_th(end)*0]; 

%delta_a=  triangle_ref(t,1e-2);
%delta_th= triangle_ref(t,1e-4);

% --- Compute speeds --- %
v=   delta_a/Ts;
om=  delta_th/Ts;

% --- Showing Trajectory on the plane --- %

x_tr= 0*t; 
y_tr= 0*t; 

for i= 1:length(delta_a)-1
    
    x_tr(i+1)= x_tr(i) + delta_a(i)*cos(theta(i));
    y_tr(i+1)= y_tr(i) + delta_a(i)*sin(theta(i));
    
end

figure(1)

plot(x_tr,y_tr)
axis equal

% ---------------- Connecting WiFi Module ----------------- %

%  Some bash to automatically connect ESP device
! bash_script/ConnectESP.sh

% ---- Check tmp.log file to know if module is really connected to right IP ------ %
fd_tmp= fopen("tmp.log");
str_log= fgetl(fd_tmp);
fclose(fd_tmp);

delete tmp.log

if strcmp(str_log,'ESP connected!')
    disp(str_log)
else
    disp(str_log)
    return
end

% -------- Opening port, start communication and reading values ----------- %

instrreset

% Opening file descriptor for the TCPIP connection
fd_s= tcpip("192.168.4.1",23); 
fopen(fd_s); 

pause(1)


% Risoluzione Schermo
tmp= get(0,'screensize');
screensize= tmp(3:4);

fig2= figure(2); clf
fig2.NumberTitle='off'; 
fig2.Name= 'Animazione';
fig2.Position= [screensize(1)/6+1 screensize(2)/10 screensize(1)/1.5-1 screensize(2)/1.3];

traj_target_line= plot(NaN,NaN); hold on
traj_measure_line= plot(NaN,NaN,'-.');
robot_target_line=  plot(NaN,NaN);
robot_measure_line=  plot(NaN,NaN);
%legend("Target","Measure")
axis equal
axis([-1 1 -1 1])

X_pt_target= [];
Y_pt_target= [];

X_pt_measure= [];
Y_pt_measure= [];

%x_tr= 0;
%y_tr= 0;
th_tr= theta; 

% Starting the script time counter
tic_global= tic;
tic_local= tic;

pose_target=  [0; 0; 0];
pose_measure= [0; 0; 0];

theta_prev= 0;
for i= 1:length(delta_a)
    
%{    
    % Send csv string
    %str= sprintf("%.15f,%.15f,\n", v(i)+delta_a_fdbk*0, om(i)+delta_th_fdbk*0);
    
    %str= str+str;
    %str= str+str;
    
    %fwrite(fd_s,str);
%}
    
    % ------ Send binary packet twice every sample ---- %
    raw_packet= [inf, v(i), om(i)];
    raw_packet= [raw_packet, raw_packet];

    fwrite(fd_s,raw_packet,'float32');
    
    % -- Blocking until new received value (for syncronization and system feedback)
    in_str= fgetl(fd_s);
    
    
    % -- Update mesaured values 
    
    % Convert input csv string in numbers
    tmp= str2double(split(in_str,','));
    
    % Rejecting stream errors
    if length(tmp) == 3 && norm(tmp) < 1e2
        pose_measure= tmp;
    end
    
    X_pt_measure= [X_pt_measure, pose_measure(1)];
    Y_pt_measure= [Y_pt_measure, pose_measure(2)];
    
    % -- Update target values 
   
    pose_target= [x_tr(i); y_tr(i); th_tr(i)];
    
    X_pt_target= [X_pt_target, pose_target(1)];
    Y_pt_target= [Y_pt_target, pose_target(2)];
    
    
    % -- Update plots
    
    % Limit plot drawing frequency
    if ~mod(i,5)
        traj_target_line.XData= X_pt_target;
        traj_target_line.YData= Y_pt_target;
    
        traj_measure_line.XData= X_pt_measure;
        traj_measure_line.YData= Y_pt_measure;

        PlotRobot(pose_measure,1,0.1,robot_measure_line);
        PlotRobot(pose_target, 2,0.1,robot_target_line);

        drawnow limitrate
    end
    
    i
    
end

toc(tic_global)

% Closing Serial file descriptor
fclose(fd_s);

return

%% Remove paths

rmpath('utils')
