clc
clear
close all

addpath("ScriptUtili/")


% ------ Time parameters ------- %

% Number of samples 
N= 1400;

% Sampling time, must be the same with STM32 project
Ts= 0.01; 

% Vector time
t= (0:N-1)*Ts; 

% ------ Mechanical Parameters -------------- %

r=     0.0695/2; % Radius of wheels
l=     0.156/2;  % Half distance between wheels
speed=   16;     % Wheels max speed (rad/s)


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
theta= [smooth_ref(t(1:end/2),0,3*pi), smooth_ref(t(1:end/2),3*pi,0)]; 

%{
a=     flip(a);
theta= flip(theta);
%}

% --- Trajectory imcrements ------ %

delta_a=  diff(a);      delta_a=  [delta_a  delta_a(end) ];
delta_th= diff(theta);  delta_th= [delta_th delta_th(end)]; 


% --- Showing Trajectory on the plane --- %

x= 0*t; 
y= 0*t; 

for i= 1:N-1
    
    x(i+1)= x(i) + delta_a(i)*cos(theta(i));
    y(i+1)= y(i) + delta_a(i)*sin(theta(i));
end

figure(1)

plot(x,y)
axis equal


% ------- C header file code generation, on STM32 project directories -------- %

% Worksapce and current project locations
workspace_dir= "~/Desktop/STM32/STM32CubeIDE/Progetto_Differential_Drive/";

project_dir= {"Differential_Drive_Trajectory_Control"};

for kk= 1:numel(project_dir)
    
    curr_proj= project_dir{kk};
    curr_time= datestr(now);
    
    % --------- Reference header -------- %
    
    % C header file on STM32 Cube IDE project
    filename= "reference.h";
    filepath= workspace_dir + curr_proj + "/Core/Inc/my_data/" + filename;
    fid= fopen(filepath,'w+');
    
    % File header
    fprintf(fid,"/*\n * "+filename+"\n *\n *  Created on: " + curr_time + "\n *      ");
    fprintf(fid,"Author: peppe\n */\n\n#ifndef INC_REFERENCE_H_\n#define INC_REFERENCE_H_");
    fprintf(fid,"\n\n");
    
    input= {delta_a, delta_th};
    names= {"DELTA_A_ref","DELTA_THETA_ref"};
    
    fprintf(fid,"\n#define N_SAMPLE_REF %d\n\n",N);
    
    % C syntax vectors
    for j= 1:length(input)
        %fprintf(fid,"float "+ names(j) + "["+num2str(N)+"]= {\n\t");
        fprintf(fid,"#define " + names(j) + " (float["+num2str(N)+"]) {\\\n\t");
        for i=1:N
            fprintf(fid,"%f, ",input{j}(i));
            if ~mod(i,5) && i ~= N
                %fprintf(fid,"\n\t");
                fprintf(fid,"\\\n\t");
            end
        end
        %fprintf(fid,"\n};\n\n");
        fprintf(fid,"\\\n}\\\n\n");
    end
    
    fprintf(fid,"#endif /* INC_REFERENCE_H_ */\n");
    fclose(fid);
    
    
end

