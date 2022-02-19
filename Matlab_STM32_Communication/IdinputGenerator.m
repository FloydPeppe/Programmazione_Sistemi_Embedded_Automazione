clc
close all
clear

% ------ Time parameters ------- %

% Number of samples 
N= 1000;

% Sampling time, must be the same with STM32 project
Ts= 0.01; 

% Vector time
t= (0:N-1)*Ts;   


% ------ Function handles for basic references --------- %

% Ramp reference, i.e. line from x_o to x_f
ramp_ref= @(t,xo,xf) xo + (xf-xo)*(t-t(1))/(t(end)-t(1));

% Triangle reference, max_val in the center 
triangle_ref= @(t,max_val) [ ramp_ref(t(1:fix(end/2)),0,1), ramp_ref(t(fix(end/2)+1:end),1,0) ]*max_val;

% Smooth step reference (from x_o to x_f)
smooth_ref= @(t,xo,xf) xo + (t-t(end)/(2*pi)*sin(2*pi*t/t(end))).*(xf-xo)/t(end);


% ------ Input signals -------- %

% Sub portion of time vector
n= 16;
M= fix(N/n);

% Triangular input
u_triangle_L= [ zeros(1,N-(2*fix(n/2-1)*M)), triangle_ref(t(1:fix(n/2-1)*M),1), triangle_ref(t(1:fix(n/2-1)*M),-1) ];
u_triangle_R= [ triangle_ref(t(1:fix(n/2-1)*M),1), triangle_ref(t(1:fix(n/2-1)*M),-1), zeros(1,N-(2*fix(n/2-1)*M))];

% Supplementary variables
u_smooth_pulse_L=  [ zeros(1,N-(2*fix(n/2-1)*M)), smooth_ref(t(1:fix(n/2-1)*M),0,1), smooth_ref(t(1:fix(n/2-1)*M),1,0) ];
u_smooth_pulse_R=  [ smooth_ref(t(1:fix(n/2-1)*M),0,1), smooth_ref(t(1:fix(n/2-1)*M),1,0), zeros(1,N-(2*fix(n/2-1)*M))];

% Smooth input
u_smooth_L= [diff(u_smooth_pulse_L) 0]/max(diff(u_smooth_pulse_L));
u_smooth_R= [diff(u_smooth_pulse_R) 0]/max(diff(u_smooth_pulse_R));


% ------- Generating C syntax header file to import data on STM32 flash -------- %

input= {u_smooth_L, u_smooth_R, u_triangle_L, u_triangle_R};
names= {"SMOOTH_input_L", "SMOOTH_input_R", "TRIANGLE_input_L", "TRIANGLE_input_R"};


% Worksapce and current project locations
workspace_dir= "~/Desktop/STM32/STM32CubeIDE/Progetto_Differential_Drive/";
project_dir= "Differential_Drive_Trajectory_Control/";

% C header file on STM32 Cube IDE project
filename= "idinput.h";
filepath= workspace_dir + project_dir + "Core/Inc/my_data/" + filename;
fid= fopen(filepath,'w+');

% File header
curr_time= datestr(now);
fprintf(fid,"/*\n * "+filename+"\n *\n *  Created on: " + curr_time + "\n *      ");
fprintf(fid,"Author: peppe\n */\n\n#ifndef INC_IDINPUT_H_\n#define INC_IDINPUT_H_");
fprintf(fid,"\n\n");

fprintf(fid,"\n#define N_SAMPLE_ID %d\n\n",N);

% C syntax vectors
for j= 1:length(input)

    fprintf(fid,"#define " + names(j) + " (float["+num2str(length(input{j}))+"]) {\\\n\t");    
    for i=1:length(input{j})
        fprintf(fid,"%f, ",input{j}(i));
        if ~mod(i,5) && i ~= N
            fprintf(fid,"\\\n\t");
        end
    end
    fprintf(fid,"\\\n}\\\n\n");
end

fprintf(fid,"#endif /* INC_DATA_H_ */\n");
fclose(fid);
