
% =========================================================================
% 
%         Writing values to serial port connected to STM32 board 
%
% =========================================================================

clc
clear
close all

% Number of Sample for the acquisition
MAX_SAMPLE= 1e3;

% Max val from analog2digital conversion with 12bit resolution
MAX_ADC_VAL= 2^12-1;

% Max filtered val
MAX_FLTRD_VAL= 180;

% Resetting connection
instrreset;

% Hardware info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_s= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_s);

% Time vector
Fs= 1e2;
t= 0:1/Fs:60;

t0= tic;
%for k= t
    tmp= tic;
   % data= 45+30*sin(2*pi*2*k);
    
    data= 190;
    
    str= compose("%7.2f",data)
    
    fwrite(fd_s,str)
    
    pause(1/Fs-toc(tmp))
    
%end
t1= toc(t0)

% Closing Serial file descriptor
fclose(fd_s);
