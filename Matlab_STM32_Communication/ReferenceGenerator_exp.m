clc
close all
clear

%% 
FileSystemConf_Init;


%%
N= 1400;

Ts= 0.01;
t= (0:N-1)*Ts;

% Mechanical Parameters
r= 0.0695/2; 
l= 0.156/2;   %0.127/2; 
%k_R= 0.0788; 
%k_L= 0.07003;

smooth_ref= @(t,xo,xf) xo + (t-t(end)/(2*pi)*sin(2*pi*t/t(end))).*(xf-xo)/t(end);
linear_ref= @(t,xo,xf) xo + (xf-xo)*t/t(end);
step_ref= @(t,xo,xf) t*0 + xf;
parabol_1_ref= @(t,xo,xf) xo + (xf-xo)*t.^2/t(end)^2;
parabol_2_ref= @(t,xo,xf) xf - (xf-xo)*(t-t(end)).^2/t(end)^2;


spline_ref= @(t,xo,xf) xo + (xf-xo) * [ parabol_1_ref(t(1:fix(end/3)),0,1/4) ...
                          1/4 + linear_ref(t(fix(end*1/3)+1:fix(end*2/3))-t(fix(end/3)),0,1)*2*1/4 ...
                             parabol_2_ref(t(fix(end*2/3)+1:end)-t(fix(end*2/3)),1/4+2*(1/4-0),1/4+3*(1/4-0))];


%y_A= [smooth_ref(t(1:fix(N/2)),0,0.3037) smooth_ref(t(fix(N/2)+1:end),0.3037,0.3037)];
%x_A= y_A;
%theta= [smooth_ref(t(1:fix(N/3)),0,pi/2) smooth_ref(t(fix(N/3)+1:fix(N*2/3))-t(fix(N/3)+1),pi/2,pi/2),...
%        smooth_ref(t(fix(N*2/3)+1:end)-t(fix(N*2/3)),pi/2,pi)];

%{
x_A= linear_ref(t,0,sqrt(2)*pi*0.4*2);
y_A= linear_ref(t,0,sqrt(2)*pi*0.4*2);
theta= linear_ref(t,0,2*pi);
%}
                         
                         
%{                       
x_A= spline_ref(t,0,sqrt(2)*pi*0.4);
y_A=  spline_ref(t,0,sqrt(2)*pi*0.4);
theta= spline_ref(t,0,2*pi);
%}


%%{              
L= spline_ref(t,0,4*pi*0.15);
theta= diff(smooth_ref(t,pi,2*pi)); theta= [theta theta(end)]*1000;
%theta= cos(10*t).*L/L(end)*2*pi;

%L= [smooth_ref(t(1:end/2),0,2*pi*0.12) smooth_ref(t(1:end/2),2*pi*0.12,2*pi*0.12)]; L= flip(L);
%L= spline_ref(t,0,8*0.20); %L= flip(L);
L= smooth_ref(t,0,1.5*pi*1*0.20); %L= flip(L);
theta= [smooth_ref(t(1:end/2),0,3*pi) smooth_ref(t(1:end/2),3*pi,0)]; %theta= flip(theta);

%L=     smooth_ref(t,0,0.34*pi*0+2*pi*r*8);
%theta= -smooth_ref(t,0,2*pi)*0;

delta_a=  diff(L);      delta_a=  [delta_a  delta_a(end) ];
delta_th= diff(theta);  delta_th= [delta_th delta_th(end)]; %delta_a.*1./(1.075+1.75/2*cos(2*pi*3.8*t/t(end)));

%}



%{
delta_a= ([diff(x_A) x_A(end)-x_A(end-1)].^2 + [diff(y_A) y_A(end)-y_A(end-1)].^2).^0.5;
delta_th= [diff(theta) theta(end)-theta(end-1)];
%}

Reference_R= t*0;
Reference_L= t*0;

for i=1:N-1
    Reference_R(i+1)= Reference_R(i) + 1920/(2*pi)*(delta_a(i)/r + delta_th(i)*l/r);
    Reference_L(i+1)= Reference_L(i) + 1920/(2*pi)*(delta_a(i)/r - delta_th(i)*l/r);
end

load Exports/Identified_Model.mat

%%{

a_R= Identified_Model_R.A(1);
b_R= Identified_Model_R.A(2);
c_R= Identified_Model_R.A(3);  % -b_R-1; 
k_R= Identified_Model_R.B(3);

a_L= Identified_Model_L.A(1);
b_L= Identified_Model_L.A(2);
c_L= Identified_Model_L.A(3);  % -b_L-1; 
k_L= Identified_Model_L.B(3);


Input_R= (a_R*[Reference_R(3:end) Reference_R(end) Reference_R(end)] + ...
          b_R*[Reference_R(2:end) Reference_R(end)]                  + ...
          c_R* Reference_R(1:end))/k_R;

Input_L= (a_L*[Reference_L(3:end) Reference_L(end) Reference_L(end)] + ...
          b_L*[Reference_L(2:end) Reference_L(end)]                  + ...
          c_L* Reference_L(1:end))/k_L;
 
Input_L(Input_L>4)=4; Input_L(Input_L<-4)=-4;
Input_R(Input_R>4)=4; Input_R(Input_R<-4)=-4;

%}

%{

k_L= Identified_Model_L.B(2);
k_R= Identified_Model_R.B(2);

a_L=  Identified_Model_L.A(2);
a_R=  Identified_Model_R.A(2);

Input_L= ([Reference_L(2:end) Reference_L(end)] +a_L*Reference_L(1:end))/k_L;
Input_R= ([Reference_R(2:end) Reference_R(end)] +a_R*Reference_R(1:end))/k_R;
%}

save("Exports/Reference.mat","Input_L","Input_R","Reference_L","Reference_R","l","r");

%%{
na_L= na+1;
nb_L= nb;
nk_L= nk;
na_R= na+1;
nb_R= nb;
nk_R= nk;

arma_L_A= Identified_Model_L.A;
arma_L_B= Identified_Model_L.B; arma_L_B= arma_L_B(end-nb_L+1:end); 
arma_R_A= Identified_Model_R.A;
arma_R_B= Identified_Model_R.B; arma_R_B= arma_R_B(end-nb_L+1:end);
%}


% Worksapce and current project locations
workspace_dir= "~/Desktop/STM32/STM32CubeIDE/Progetto_Differential_Drive/";

project_dir= {"Differential_Drive_Trajectory_Control","Differential_Drive_Thread_New", ...
              "Remote_Control_Joystick","Remote_Control","Phone_Remote_Controller"};

%project_dir= {"Differential_Drive_Trajectory_Control_Test"};          
          
for kk= 1:numel(project_dir)
    
    curr_proj= project_dir{kk};
    curr_time= datestr(now);
     
    % --------- Reference header -------- %
    
    if strcmp(curr_proj,"Differential_Drive_Trajectory_Control")
        
        % C header file on STM32 Cube IDE project
        filename= "reference.h";
        filepath= workspace_dir + curr_proj + "/Core/Inc/my_data/" + filename;
        fid= fopen(filepath,'w+');
        
        % File header
        fprintf(fid,"/*\n * "+filename+"\n *\n *  Created on: " + curr_time + "\n *      ");
        fprintf(fid,"Author: peppe\n */\n\n#ifndef INC_REFERENCE_H_\n#define INC_REFERENCE_H_");
        fprintf(fid,"\n\n");
        
        input= {Reference_L,Reference_R,delta_a,delta_th};
        names= {"LEFT_ref","RIGHT_ref","DELTA_A_ref","DELTA_THETA_ref"};
        
        fprintf(fid,"\n#define N_SAMPLE_REF %d\n\n",N);
        
        % C syntax vectors
        for j= 1:length(input)
            %fprintf(fid,"float "+ names(j) + "["+num2str(N)+"]= {\n\t");
            fprintf(fid,"#define " + names(j) + " (float["+num2str(N)+"]) {\\\n\t");
            for i=1:N
                fprintf(fid,"%f, ",input{j}(i));
                if ~mod(i,5) && i ~= N
                    fprintf(fid,"\\\n\t");
                end
            end
            fprintf(fid,"\\\n}\\\n\n");
        end
        
        fprintf(fid,"#endif /* INC_REFERENCE_H_ */\n");
        fclose(fid);
        
    end
    
    % ------- Identification header --------- %
    
    % C header file on STM32 Cube IDE project
    filename= "identification.h";
    filepath= workspace_dir + curr_proj + "/Core/Inc/my_data/" + filename;
    fid= fopen(filepath,'w+');
    
    % File header
    fprintf(fid,"/*\n * "+filename+"\n *\n *  Created on: " + curr_time + "\n *      ");
    fprintf(fid,"Author: peppe\n */\n\n#ifndef INC_IDENTIFICATION_DATA_H_\n#define INC_IDENTIFICATION_DATA_H_");
    fprintf(fid,"\n\n");
    
    input_scalar= {na_L,nb_L,nk_L,na_R,nb_R,nk_R};
    names_scalar= {"N_AR_L","N_MA_L","TAU_L","N_AR_R","N_MA_R","TAU_R"};
    
    % C syntax scalar
    for j= 1:length(input_scalar)
        fprintf(fid,"#define " + names_scalar(j) + "  ");
        fprintf(fid,"%d",input_scalar{j});
        fprintf(fid,"\n\n\n");
    end
    
    input_vector= {arma_L_A,arma_L_B,arma_R_A,arma_R_B};
    names_vector= {"AR_L","MA_L","AR_R","MA_R"};
    
    % C syntax vectors
    for j= 1:length(input_vector)
        fprintf(fid,"#define " + names_vector(j) + " (float["+num2str(length(input_vector{j}))+"]) {\\\n\t");
        for i=1:length(input_vector{j})
            fprintf(fid,"%.10f, ",input_vector{j}(i));
            if ~mod(i,5) && i ~= N
                fprintf(fid,"\\\n\t");
            end
        end
        fprintf(fid,"\\\n}\\\n\n");
    end
    
    
    fprintf(fid,"#endif /* INC_IDENTIFICATION_DATA_H_ */\n");
    fclose(fid);
    
end




%{

% C header file on STM32 Cube IDE project
project_dir= "~/Desktop/STM32_projects/STM32CubeIDE/workspace_1.6.1/Differential_Drive_Control/Core/Inc/my_lib/";
filename= "identification_data.h";
filepath= project_dir + filename;
fid= fopen(filepath,'w+');

% C Intestation
fprintf(fid,"/*\n * "+filename+"\n *\n *  Created on: Aug 26, 2021\n *      ");
fprintf(fid,"Author: peppe\n */\n\n#ifndef INC_IDENTIFICATED_PARAMETER_H_\n#define INC_IDENTIFICATED_PARAMETER_H_");
fprintf(fid,"\n\n");

input= {a_L,b_L,c_L,k_L,a_R,b_R,c_R,k_R};
names= {"a_L","b_L","c_L","k_L","a_R","b_R","c_R","k_R"};

% C syntax vectors
for j= 1:length(input)
    fprintf(fid,"#define " + names(j) + " (float)  ");
    fprintf(fid,"%f",input{j});
    fprintf(fid,"\n\n\n");
end

fprintf(fid,"#endif /* INC_IDENTIFICATED_PARAMETER_H_ */\n");
fclose(fid);

%}

x= 0*t; y= 0*t; th= 0*t;
x(1)= 0; 
y(1)= 0; 
th(1)= 0;
Delta_wheel_L= diff(Reference_L);
Delta_wheel_R= diff(Reference_R);
for i= 1:N-1
    
    th(i+1)= th(i) + r/(2*l)*2*pi/1920*(Delta_wheel_R(i)-Delta_wheel_L(i));
    x(i+1)= x(i) + r/2*2*pi/1920*(Delta_wheel_R(i)+Delta_wheel_L(i))*cos(th(i));
    y(i+1)= y(i) + r/2*2*pi/1920*(Delta_wheel_R(i)+Delta_wheel_L(i))*sin(th(i));
   
    
end

figure
plot(x,y)
axis equal

%%

FileSystemConf_Deinit





