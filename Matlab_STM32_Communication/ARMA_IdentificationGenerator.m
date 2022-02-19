clc
clear
close all

% --- Loading stored identification parameters ---- %

tmp= load('Exports/Identified_Model.mat');

% -- Left wheel -- %
na_L= tmp.na+1;
nb_L= tmp.nb;
nk_L= tmp.nk;

arma_L_A= tmp.Identified_Model_L.A;
arma_L_B= tmp.Identified_Model_L.B; arma_L_B= arma_L_B(end-nb_L+1:end); 

% -- Right wheel -- %
na_R= tmp.na+1;
nb_R= tmp.nb;
nk_R= tmp.nk;

arma_R_A= tmp.Identified_Model_R.A;
arma_R_B= tmp.Identified_Model_R.B; arma_R_B= arma_R_B(end-nb_L+1:end);


% ------- C header file code generation, on STM32 project directories -------- %

% Worksapce and current STM32 project locations
workspace_dir= "~/Desktop/STM32/STM32CubeIDE/Progetto_Differential_Drive/";

project_dir= {"Differential_Drive_Trajectory_Control", ...
              "Differential_Drive_Thread_New", ...
              "Remote_Control_Joystick", ...
              "Remote_Control", ...
              "Phone_Remote_Controller" ...
              };

for kk= 1:numel(project_dir)
       
    curr_proj= project_dir{kk};
    curr_time= datestr(now);
    
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
    
    % C syntax scalars
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
