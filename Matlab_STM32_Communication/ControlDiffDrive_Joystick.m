close all
clc
clear

% Resetting connection
instrreset;
  
% Opening file descriptor for the TCPIP connection
fd_tcpip= tcpip("192.168.4.1",23); 
fopen(fd_tcpip); 


% Serial info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_ser= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_ser);

pause(2)

% Start communication
fprintf(fd_ser,"\n");

N_data= 2;
str_curr= fgetl(fd_ser);
while true 
    
    str= fgetl(fd_ser)
    fprintf(fd_tcpip,str);
    
    clc
     
end

fclose(fd_ser);