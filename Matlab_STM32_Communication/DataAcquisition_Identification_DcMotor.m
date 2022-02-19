
% ========================================================================== %
%                                                                            %
%        Reading values from serial port connected to STM32 board.           % 
%                                                                            %
%   You'd better start this script before the data streaming from board      % 
%                    started, to avoid loss of data.                         % 
%                                                                            %
% ========================================================================== %

clc
clear
close all

FileSystemConf_Init

% Number of Sample for the acquisition
MAX_SAMPLE= 1200;

% Number of different data each acquisition
N_data= 5;

% Resetting connection
instrreset;

% Hardware info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_s= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_s);
    
% Defining vectors and cell
idx=           1:MAX_SAMPLE;
theta_step_L=  zeros(1,MAX_SAMPLE);
theta_step_R=  zeros(1,MAX_SAMPLE);
input_V_L=     zeros(1,MAX_SAMPLE);
input_V_R=     zeros(1,MAX_SAMPLE);
t=             zeros(1,MAX_SAMPLE);
str=           cell(1,3);

% Reading from input
for i=idx 
    
    % Read serial-in "file" line by line using fgetl. In every line there 
    % is one value
    str_curr= fgetl(fd_s);
    
    % Starting the script time counter just after the first line catched
    if i==1
        tic
    end 
    
    % Filtering the 3 values removing spaces and tabs
    tab_ch= sprintf("\t");
    j=1;
    for k= 1:N_data
        l= 1;
        while str_curr(j) ~= ' ' && str_curr(j) ~= tab_ch && j < strlength(str_curr)
            str{k}(l) = str_curr(j);
            l= l+1;
            j= j+1;
        end
        while str_curr(j) == ' ' || str_curr(j) == tab_ch
            j=j+1;
        end
    end  
   
    % Converting values
    theta_step_L(i)= str2double(str{1});
    theta_step_R(i)= str2double(str{2});
    input_V_L(i)=    str2double(str{3});
    input_V_R(i)=    str2double(str{4});
    t(i)=            str2double(str{5});
    
    % Printing current values
    clc
    fprintf("Step_L  Step_R  Volt_L  Volt_R   Time\n");
    fprintf("%.1f\t",theta_step_L(i));
    fprintf("%.1f\t",theta_step_R(i));
    fprintf("%.1f\t",input_V_L(i));
    fprintf("%.1f\t",input_V_R(i));
    fprintf(" %.1f\n",t(i));
end

% Closing Serial file descriptor
fclose(fd_s);

% Elapsed time in the script
fprintf("\nElapsed time on streaming is: %f\n\n",toc);
    
%% Showing results

% Risoluzione Schermo
tmp= get(0,'screensize');
screensize= tmp(3:4);

fig1= figure(1); clf
fig1.NumberTitle='off'; 
fig1.Name= 'Misure Motori';
fig1.Position= [screensize(1)/4+1 screensize(1)/8+1 screensize(1)/2-1 screensize(2)/2];

subplot(2,2,1)
plot(t, theta_step_L,"LineWidth",1.5)
legend("Theta Steps Left")

xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel('$[\frac{2\pi}{20} rad]$',"Interpreter","latex","FontSize",15)

subplot(2,2,3)
plot(t, input_V_L,"LineWidth",1.5)
legend("Input Tension Left")
xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel("$[V]$","Interpreter","latex","FontSize",15)

subplot(2,2,2)
plot(t, theta_step_R,"LineWidth",1.5)
legend("Theta Steps Right")

xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel('$[\frac{2\pi}{20} rad]$',"Interpreter","latex","FontSize",15)

subplot(2,2,4)
plot(t, input_V_R,"LineWidth",1.5)
legend("Input Tension Right")
xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel("$[V]$","Interpreter","latex","FontSize",15)

%% Identification

% Sample time
Ts= t(2)-t(1);

%%% Dati per identificazione
data_L= iddata(theta_step_L',input_V_L',Ts);
data_R= iddata(theta_step_R',input_V_R',Ts);
    
%%% Gradi dei polinomi del modello ARMAX
na= 2; nb= 1; nc= 2; nk= 2;
    
%%% Identificazione
sys_L = armax(data_L,[na nb nc nk]);
sys_R = armax(data_R,[na nb nc nk]);

id_num_L= sys_L.B;
id_den_L= sys_L.A;

id_num_R= sys_R.B; 
id_den_R= sys_R.A;

% Load Model
tmp_var= load("Exports/Identified_Model.mat");
Identified_Model_L= tmp_var.Identified_Model_L
Identified_Model_R= tmp_var.Identified_Model_R

%{ 
% Save Model
Identified_Model_L= tf(id_num_L,id_den_L,Ts)
Identified_Model_R= tf(id_num_R,id_den_R,Ts)
save("Exports/Identified_Model","Identified_Model_L","Identified_Model_R");
%}

% Simulation
theta_step_sim_L= lsim(Identified_Model_L,input_V_L,t);
theta_step_sim_R= lsim(Identified_Model_R,input_V_R,t);

figure(3), clf

subplot(2,1,1)
plot(t, theta_step_L,"LineWidth",1.5)
hold on
plot(t, theta_step_sim_L,"LineWidth",1.5)
legend("Theta step Misurata","Theta step Simulata")

subplot(2,1,2)
plot(t, theta_step_R,"LineWidth",1.5)
hold on
plot(t, theta_step_sim_R,"LineWidth",1.5)
legend("Theta step Misurata","Theta step Simulata")

%% Control

%{
ref_L= 20*(1-cos(2*pi*0.4*t))/2;
%{
N= MAX_SAMPLE/2;
x_o= 0;
x_f= 120;
M= round(N*0.8);
t_cut= t(1:M);
u_smooth= x_o + (t_cut-t(M)/(2*pi)*sin(2*pi*t_cut/t(M)))*(x_f-x_o)/t(M);
u_smooth= [u_smooth t(M+1:N)*0+x_f];
%}
%u_step= t*0+x_f;

u_step= [t(1:round(N/10))*0 t(round(N/10)+1:N-round(N/5))*0+x_f t(N-round(N/5)+1:N)*0];
u_step= t*0+40;

u_smooth_L= [u_smooth  x_f+u_smooth];
u_smooth_R= [u_smooth  x_f-u_smooth];

ref_L= u_smooth_L;
ref_R= u_smooth_R;
%{
rad_step= 1;
for i=1:100
    ref( ref > (i-1)*rad_step & ref < i*rad_step )= (i-1)*rad_step;
end
%}

delta_ref= [diff(ref_L) 0];

%{
A= 1;
B= sys_L.B(2);

K= acker(A,B,0.1);

sim("SimulinkModel/DcMotor_Control")

theta_step_ctrl= theta_step_out.Data';

figure(4), clf

plot(t,[theta_step_ctrl' ref'])
legend("Controllo Simulato","Riferimento")

%}
%}

load("Exports/Reference.mat");

ref_L= Reference_L;
ref_R= Reference_R;

fig5= figure(5); clf
fig5.NumberTitle='off'; 
fig5.Name= 'Confronti';
fig5.Position= [screensize(1)/4+1 screensize(1)/8+1 screensize(1)/2-1 screensize(2)/2];

subplot(2,2,1)
plot(t,[theta_step_L' ref_L'])
legend("Theta Steps Left","Target")
xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel('$[\frac{2\pi}{20} rad]$',"Interpreter","latex","FontSize",15)

subplot(2,2,3)
plot(t,[theta_step_R' ref_R'])
legend("Theta Steps Right","Target")
xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel('$[\frac{2\pi}{20} rad]$',"Interpreter","latex","FontSize",15)

subplot(2,2,[2;4])
plot(t,theta_step_L-theta_step_R)
legend("Left-Right Differences")
xlabel("$[s]$","Interpreter","latex","FontSize",15)
ylabel('$[\frac{2\pi}{20} rad]$',"Interpreter","latex","FontSize",15)


x_r= 0*t; y_r= 0*t; th_r= 0*t;
x_v= 0*t; y_v= 0*t; th_v= 0*t;
for i= 2:MAX_SAMPLE
    
    th_r(i)= th_r(i-1) + r/(2*l)*pi/10*(ref_R(i)-ref_R(i-1)-ref_L(i)+ref_L(i-1));
    x_r(i)= x_r(i-1) + r/2*pi/10*(ref_R(i)-ref_R(i-1)+ref_L(i)-ref_L(i-1))*cos(th_r(i));
    y_r(i)= y_r(i-1) + r/2*pi/10*(ref_R(i)-ref_R(i-1)+ref_L(i)-ref_L(i-1))*sin(th_r(i));
   
    th_v(i)= th_v(i-1) + r/(2*l)*pi/10*(theta_step_R(i)-theta_step_R(i-1)-theta_step_L(i)+theta_step_L(i-1));
    x_v(i)= x_v(i-1) + r/2*pi/10*(theta_step_R(i)-theta_step_R(i-1)+theta_step_L(i)-theta_step_L(i-1))*cos(th_v(i));
    y_v(i)= y_v(i-1) + r/2*pi/10*(theta_step_R(i)-theta_step_R(i-1)+theta_step_L(i)-theta_step_L(i-1))*sin(th_v(i));
    
    %x(i)= x(i-1)+delta_p(i)*cos(theta(i));
    %y(i)= y(i-1)+delta_p(i)*sin(theta(i));
    
end

fig6= figure(6); clf
fig6.NumberTitle='off'; 
fig6.Name= 'Traiettorie';
fig6.Position= [screensize(1)/4+1 screensize(1)/8+1 screensize(1)/2-1 screensize(2)/2];

subplot(1,2,1)
plot(x_r,y_r)
hold on
axis equal
plot(x_v,y_v)
title("Trajectory")
legend("Riferimento","Vero")

subplot(1,2,2)
plot(t,[th_r' th_v'])
title("Theta Angle")
legend("Riferimento","Vero")

return

%%

FileSystemConf_Deinit;


