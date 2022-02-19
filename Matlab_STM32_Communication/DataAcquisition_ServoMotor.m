
% =========================================================================
% 
%  STM32 board is reading value from a potentiometer using an ADC, then 
%  the raw input is converted to a value from 0 to 180 to be used for a
%  servo motor. The values are then filterd using a MovingAverageFilter.
%  Using a serial connection, the matlab script, can read the raw values
%  and the filtered ones and then show the differences. You'd better reset
%  the board just before to start this script, the microcontroller it's   
%  set to wait 3 seconds before starting the serial stream. This help  
%  avoiding loss of values and errors.
%
%  On STM32IDE the project is called "Eser_9_Servo_motor_Input_from_pot_"
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
    
% Defining vectors and cell
idx=        1:MAX_SAMPLE;
fltrd_val=  zeros(1,MAX_SAMPLE);
adc_val=    zeros(1,MAX_SAMPLE);
t=          zeros(1,MAX_SAMPLE);
str=        cell(1,3);

% Reading from input
for i=idx 
    
    % Read serial-in "file" line by line using fgetl. In every line there 
    % are three values: filetered, raw from ADC and the current time. So, 
    % from stm32, you need to print every value separeted with spaces and 
    % a final newline char '\n' ending the current samples.
    
    str_curr= fgetl(fd_s);
    
    % Starting the script time counter just after the first line catched
    if i==1
        tic
    end 
    
    % Filtering the 3 values removing spaces and tabs
    tab_ch= sprintf("\t");
    j=1;
    for k= 1:3
        l= 1;
        while str_curr(j) ~= ' ' && str_curr(j) ~= tab_ch && j < strlength(str_curr)
            str{k}(l) = str_curr(j);
            l= l+1;
            j= j+1;
        end
        while str_curr(j) == ' ' || str_curr(j) == tab_ch
            j=j+1;
        end
    end  
   
    % Converting values
    fltrd_val(i)=   str2double(str{1});
    adc_val(i)=     str2double(str{2});
    t(i)=           str2double(str{3});
    
    % Printing current values
    clc
    fprintf("Degree  ADC Val  Time\n");
    fprintf("%.1f\t",fltrd_val(i));
    fprintf("%d\t",adc_val(i));
    fprintf(" %.1f\n",t(i));
end

% Closing Serial file descriptor
fclose(fd_s);

% Elapsed time in the script
fprintf("\nElapsed time on streaming is: %f\n\n",toc);

% Normalizing value read from ADC to be comparised with the filtered ones
adc_val= adc_val/MAX_ADC_VAL*MAX_FLTRD_VAL;

%% Showing result

plot(t,[fltrd_val; adc_val],"LineWidth",1.5)
legend("Filtered Values","Raw Values")
