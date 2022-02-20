
clc
clear
close all

fprintf("Press the blue button on STM32 board in order to start data streaming\n\n");

% Variable used to change code behaviour
IDENTIFICATION_ONLY= 0;

% --- Import saved parameters ---- %

imported= load("Exports/parameters.mat");

% ---------------- Time Variables -------------------- % 

% Number of Sample for the acquisition
N_sample= 1400;

% Time step
Ts= imported.prmtrs.TIME_STEP.val;

% Time vector
t= (0:N_sample-1)*Ts;

% --------- Data variables declaration --------------- %

% Number of different data each acquisition
N_data= 18;

% Defining vectors and cell
init_vec= zeros(1,N_sample);

idx=            1:N_sample;
input_L_real=   init_vec;
input_R_real=   init_vec;
input_L_goal=   init_vec;
input_R_goal=   init_vec;
L_real=         init_vec;
R_real=         init_vec;
L_goal=         init_vec;
R_goal=         init_vec;
xa_real=        init_vec;
ya_real=        init_vec;
theta_real=     init_vec;
v_real=         init_vec;
om_real=        init_vec;
xa_goal=        init_vec;
ya_goal=        init_vec;
theta_goal=     init_vec;
v_goal=         init_vec;
om_goal=        init_vec;

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

% Opening file descriptor for the TCPIP connection
fd_s= tcpip("192.168.4.1",23); 
fopen(fd_s); 

% Send at least 10 char to start communication
fprintf(fd_s,"ciaomondo111");

% Read serial-in "file" line by line using fgetl
str_curr= fgetl(fd_s);
while isempty(str_curr)    
    str_curr= fgetl(fd_s);
end

% First element is number of samples
N_sample_curr= fix(str2double(str_curr));
    
% Starting the script time counter 
tic

% First line of values
str_curr= fgetl(fd_s);

i= 1;
while ~strcmp(str_curr,'') 

    % Convert read csv string in numbers
    tmp= str2double(split(str_curr,','));
    
    % Rejecting stream errors
    if length(tmp) == N_data
        data_in= tmp;
    end
    
    % Converting values
    input_L_real(i)=  data_in(1);
    input_R_real(i)=  data_in(2);
    input_L_goal(i)=  data_in(3);
    input_R_goal(i)=  data_in(4);
    L_real(i)=        data_in(5);
    R_real(i)=        data_in(6);
    L_goal(i)=        data_in(7);
    R_goal(i)=        data_in(8);
    xa_real(i)=       data_in(9);
    ya_real(i)=       data_in(10);
    theta_real(i)=    data_in(11);
    v_real(i)=        data_in(12);
    om_real(i)=       data_in(13);
    xa_goal(i)=       data_in(14);
    ya_goal(i)=       data_in(15);
    theta_goal(i)=    data_in(16);
    v_goal(i)=        data_in(17);
    om_goal(i)=       data_in(18);
    
    % Printing current values
    clc
    fprintf("Volt_L  Volt_R  Sens_L  Sens_R  Ref_L   Ref_R\n");
    fprintf("%.1f\t",input_L_real(i));
    fprintf("%.1f\t",input_R_real(i));
    fprintf("%.1f\t",L_real(i));
    fprintf("%.1f\t",R_real(i));
    fprintf("%.1f\t",L_goal(i));
    fprintf("%.1f\t",R_goal(i));

    % Increment index
    i= i+1;
    
    % Break if N sample are reached
    if i > N_sample_curr
        break
    end
    
    % Read serial-in "file" line by line using fgetl
    str_curr= fgetl(fd_s);
    
end
last_time= toc;

% Completing last values
for j= i:N_sample

    input_L_real(j)=  input_L_real(j-1);
    input_R_real(j)=  input_R_real(j-1);
    input_L_goal(j)=  input_L_goal(j-1);
    input_R_goal(j)=  input_R_goal(j-1);
    L_real(j)=        L_real(j-1);
    R_real(j)=        R_real(j-1);
    L_goal(j)=        L_goal(j-1);
    R_goal(j)=        R_goal(j-1);
    xa_real(j)=       xa_real(j-1);
    ya_real(j)=       ya_real(j-1);
    theta_real(j)=    theta_real(j-1);
    v_real(j)=        v_real(j-1);
    om_real(j)=       om_real(j-1);
    xa_goal(j)=       xa_goal(j-1);
    ya_goal(j)=       ya_goal(j-1);
    theta_goal(j)=    theta_goal(j-1);
    v_goal(j)=        v_goal(j-1);
    om_goal(j)=       om_goal(j-1);
end

% Closing Serial file descriptor
fclose(fd_s);

% Elapsed time in the script
fprintf("\n\nElapsed time on streaming is: %f\n\nShowing results\n\n",last_time);

%% Identification

if IDENTIFICATION_ONLY
    
    %%% Dati per identificazione
    data_L= iddata(L_real',input_L_real',Ts); data_L= misdata(data_L);
    data_R= iddata(R_real',input_R_real',Ts); data_R= misdata(data_R);
    
    %%% Gradi dei polinomi del modello ARMAX
    na= 2; nb= 1; nc= 2; nk= 2;
    
    %%% Identificazione
    sys_L = armax(data_L,[na nb nc nk]);
    sys_R = armax(data_R,[na nb nc nk]);
    
    z= tf('z',Ts);
    
    % Load previous Model
    %tmp_var= load("Exports/Identified_Model.mat");
    %Identified_Model_L= tmp_var.Identified_Model_L;
    %Identified_Model_R= tmp_var.Identified_Model_R;
    
    % Show Model
    Identified_Model_L= sys_L
    Identified_Model_R= sys_R
    
    % Simulation
    L_sim= lsim(Identified_Model_L,input_L_real,t);
    R_sim= lsim(Identified_Model_R,input_R_real,t);
    
    figure(5), clf
    
    subplot(2,1,1)
    plot(t, L_real,"LineWidth",1.5)
    hold on
    plot(t, L_sim,"LineWidth",1.5)
    title("LEFT")
    legend("Step Ruota Misurata","Step Ruota Simulata")
    
    subplot(2,1,2)
    plot(t, R_real,"LineWidth",1.5)
    hold on
    plot(t, R_sim,"LineWidth",1.5)
    title("RIGHT")
    legend("Step Ruota Misurata","Step Ruota Simulata")
  
    fprintf("\nPress any Key to update Scripts on STM32IDE\n");
    pause
    
    % Save model
    save("Exports/Identified_Model","Identified_Model_L","Identified_Model_R","na","nb","nk");
   
    % Call control_parameters Code Generator to update STM32 scripts
    run Code_Generator_Scripts/control_parameters_Generator.m
    return
    
end

%% Showing result
    
l= imported.prmtrs.WHEEL_DISTANCE.val; % Half distance between wheels
r= imported.prmtrs.WHEEL_RADIUS.val;   % Radius of wheels

figure(1),clf

subplot(2,2,1)
plot(t,L_real,"LineWidth",1.5)
legend("Left Wheel Position")


subplot(2,2,3)
plot(t,input_L_real,"LineWidth",1.5)
legend("Left Input Voltage")

subplot(2,2,2)
plot(t,R_real,"LineWidth",1.5)
legend("Right Wheel Position")

subplot(2,2,4)
plot(t,input_R_real,"LineWidth",1.5)
legend("Right Input Voltage")


figure(2),clf

subplot(2,1,1)
plot(t,[L_real; L_goal],"LineWidth",1.5)
title("LEFT")
legend("Real Values","Target Values")


subplot(2,1,2)
plot(t,[R_real; R_goal],"LineWidth",1.5)
title("RIGHT")
legend("Real Values","Target Values")

%%{
figure(6),clf

subplot(2,1,1)
plot(t,[input_L_real; input_L_goal],"LineWidth",1.5)
legend("Real Values","Simulated Values")
axis([t(1) t(end) -8 8])
title("Left Input")

subplot(2,1,2)
plot(t,[input_R_real; input_R_goal],"LineWidth",1.5)
legend("Real Values","Simulated Values")
axis([t(1) t(end) -8 8])
title("Right Input")
%}


%% Trajectory

delta_th_real= diff(theta_real);
delta_a_real=  (diff(R_real)+diff(L_real))*r/2;

delta_th_goal= diff(theta_goal);
delta_a_goal=  (diff(R_goal)+diff(L_goal))*r/2;

% Risoluzione Schermo
tmp= get(0,'screensize');
screensize= tmp(3:4);

fig3= figure(3); clf
fig3.NumberTitle='off'; 
fig3.Name= 'Traiettorie';
fig3.Position= [screensize(1)/4+1 screensize(1)/8+1 screensize(1)/2-1 screensize(2)/2];

subplot(1,2,1)
plot(xa_goal,ya_goal)
hold on
axis equal
plot(xa_real,ya_real)
title("Trajectory")
legend("Riferimento","Vero")
xlabel("[m]"), ylabel("[m/s]")
axis equal
    
subplot(1,2,2)
plot(t,[theta_goal' theta_real'])
title("Orientation")
legend("Riferimento","Vero")
xlabel("[s]"), ylabel("[rad]")

fig4= figure(4); clf
fig4.NumberTitle='off'; 
fig4.Name= 'Trajectory Increments';
fig4.Position= [screensize(1)/4+1 screensize(1)/8+1 screensize(1)/2-1 screensize(2)/2];

subplot(1,2,1)
plot(t,v_goal*100)
hold on
plot(t,v_real*100)
axis([t(1) t(end) -4e1 4e1])
title("Linear speed")
legend("Goal","Real")
xlabel("[s]"), ylabel("[cm/s]") 

subplot(1,2,2)
plot(t,om_goal)
hold on
plot(t,om_real)
axis([t(1) t(end) -5e0 5e0])
title("Angular speed")
legend("Goal","Real")
xlabel("[s]"), ylabel("[rad/s]")

