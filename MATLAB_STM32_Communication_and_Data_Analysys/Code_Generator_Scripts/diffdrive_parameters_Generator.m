clc
close all
clear

% Import parameters
tmp= load("../Exports/parameters.mat");
prmtrs= tmp.prmtrs;

% ------- Generating C syntax header file to import data on STM32 project -------- %

names= fieldnames(prmtrs);

% Worksapce and current project locations
workspace_dir= "../../STM32cubeIDE_Workspace/";
project_dir= {"Identification_and_Trajectory_Tracking/", "Autonomous/", "Remote_Controller/", ...
              "Matlab_Controller/"};

for kk= 1:numel(project_dir)
    
    curr_proj= project_dir{kk};
    
    % C header file on STM32 Cube IDE project
    filename= "diffdrive_parameters.h";
    filepath= workspace_dir + curr_proj + "Core/Inc/my_data/" + filename;
    fid= fopen(filepath,'w+');
    
    % File header
    curr_time= datestr(now);
    fprintf(fid,"/*\n * "+filename+"\n *\n" + ...
        " *  Generated from MATLAB \n"    + ...
        " *  " + curr_time + "\n *      ");
    fprintf(fid,"\n */\n\n#ifndef INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_\n" + ...
        "#define INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_");
    fprintf(fid,"\n\n");
    
    % C syntax preprocessor defines
    for i= 1:numel(names)
        
        fprintf(fid,"/*\n" + prmtrs.(names{i}).comment + "\n*/\n");
        fprintf(fid,"#define " + names{i} + " %ff", prmtrs.(names{i}).val);
        fprintf(fid,"\n\n");
    end
    
    fprintf(fid,"\n\n");
    
    fprintf(fid,"#endif /* INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_ */\n");
    fclose(fid);
    
end
