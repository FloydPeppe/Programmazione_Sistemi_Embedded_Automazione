close all
clc
clear

% Resetting connection
instrreset;
  
% Opening file descriptor for the TCPIP connection
fd_tcip= tcpip("192.168.4.1",23); 
fopen(fd_tcip); 
    
while true
    
    ! read -n1 x && echo $x > tmp   
    fd= fopen("tmp");
    ch= fgets(fd);
    fclose(fd);
    
    fprintf(fd_tcip,"%c", ch);
   
    pause(0.01);
    clc
end