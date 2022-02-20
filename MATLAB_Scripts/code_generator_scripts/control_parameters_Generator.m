clc
clear
close all

% -- Import saved parameters ---- %

tmp= load("../exports/parameters.mat");
prmtrs= tmp.prmtrs;

% -- Set PID parameters
pid_P= 25; % Proportional
pid_I= 8;  % Integral
pid_D= 0;  % Derivative

% --- Loading stored identification parameters ---- %

tmp= load('../exports/Identified_Model.mat');

% -- Left wheel -- %
na_L= tmp.na+1;
nb_L= tmp.nb;
nk_L= tmp.nk;

arma_L_A= tmp.Identified_Model_L.A;
% Make first element equal to 1, second and third element summing to -1. So three coefficient sum to 0
%arma_L_A(1)= 1;
%arma_L_A(3)= -arma_L_A(2)-1;

arma_L_B= tmp.Identified_Model_L.B; arma_L_B= arma_L_B(end-nb_L+1:end); 

% -- Right wheel -- %
na_R= tmp.na+1;
nb_R= tmp.nb;
nk_R= tmp.nk;

arma_R_A= tmp.Identified_Model_R.A;
% Make first element equal to 1, second and third element summing to -1. So three coefficient sum to 0
%arma_R_A(1)= 1;
%arma_R_A(3)= -arma_R_A(2)-1;

arma_R_B= tmp.Identified_Model_R.B; arma_R_B= arma_R_B(end-nb_L+1:end);


% ------- C header file code generation, on STM32 project directories -------- %

% Worksapce and current STM32 project locations
workspace_dir= "../../STM32cubeIDE_Workspace/";

project_dir= {"Identification_and_Trajectory_Tracking/", "Autonomous/", "Remote_Controller/", ...
              "Matlab_Controller/"};

for kk= 1:numel(project_dir)
       
    curr_proj= project_dir{kk};
    curr_time= datestr(now);
    
    % C header file on STM32 Cube IDE project
    filename= "control_parameters.h";
    filepath= workspace_dir + curr_proj + "Core/Inc/my_data/" + filename;
    fid= fopen(filepath,'w+');
    
    % File header
    fprintf(fid,"/*\n * "+filename+"\n *\n" + ...
                " *  Generated from MATLAB \n"    + ...
                " *  " + curr_time + "\n *      ");
    fprintf(fid,"\n */\n\n#ifndef INC_MY_DATA_IDENTIFICATION_DATA_H_\n#define INC_IDENTIFICATION_DATA_H_");
    fprintf(fid,"\n\n");
    
    input_int_scalar= {na_L,nb_L,nk_L,na_R,nb_R,nk_R};
    names_int_scalar= {"N_AR_L","N_MA_L","TAU_L","N_AR_R","N_MA_R","TAU_R"};
    
    input_float_scalar= {pid_P,pid_I,pid_D,pid_P,pid_I,pid_D};
    names_float_scalar= {"PID_P_L","PID_I_L","PID_D_L","PID_P_R","PID_I_R","PID_D_R"};
    
 
    % C syntax scalars
    fprintf(fid,"\n");
    for j= 1:length(input_float_scalar)
        fprintf(fid,"#define " + names_float_scalar(j) + "  ");
        fprintf(fid,"%ff",input_float_scalar{j});
        fprintf(fid,"\n");
    end
    fprintf(fid,"\n\n");
    for j= 1:length(input_int_scalar)
        fprintf(fid,"#define ");
        fprintf(fid,"%6s  ", names_int_scalar{j});
        fprintf(fid,"%1d",input_int_scalar{j});
        fprintf(fid,"\n");
        
    end
    
    fprintf(fid,"\n\n");
    
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

    fprintf(fid,"\n");
    fprintf(fid,"#endif /* INC_IDENTIFICATION_DATA_H_ */\n");
    fclose(fid);
    
end
