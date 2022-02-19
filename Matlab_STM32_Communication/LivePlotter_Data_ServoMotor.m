    
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
MAX_SAMPLE= 1e4;

% Max val from analog2digital conversion with 12bit resolution
MAX_ADC_VAL= 2^12-1;

% Max filtered val
MAX_FLTRD_VAL= 180;

% Windows time size for the plot
WIND_TIME_SIZE= 5;

% Animation objects
fig= figure(1);
fig.Position= [1920/6 1080/6 1920/1.5 1080/1.5]; % Windows size and position
ax= gca;

% Lines
h=          plot(NaN,NaN(1,3),'LineWidth',1.5);
filtr_line= h(2);
raw_line=   h(1);
time_line=  h(3);

% Resetting connection
instrreset;

% Hardware info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_s= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_s);
    
% Defining vectors and cell
idx=        1:MAX_SAMPLE;
fltrd_val=  NaN(1,MAX_SAMPLE);
adc_val=    NaN(1,MAX_SAMPLE);
t=          NaN(1,MAX_SAMPLE);
str=        cell(3);

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
   
    % Converting values ---------------------------------------------------
    fltrd_val(i)=   str2double(str{1});
    % Normalizing values from ADC to be comparised with the filtered ones
    adc_val(i)=     str2double(str{2})/MAX_ADC_VAL*MAX_FLTRD_VAL;
    t(i)=           str2double(str{3});
    
    % Showing the plotter
    set(filtr_line, 'XData',t(:), 'YData',fltrd_val(:))
    set(raw_line,   'XData',t(:), 'YData',adc_val(:))

    % Sliding the plot horizontal axes
    ax.XLim(1)= t(i)-WIND_TIME_SIZE;
    ax.XLim(2)= t(i);
    
    drawnow update
end
    
% Elapsed time in the script
fprintf("\nElapsed time on streaming is: %.1f s\n\n",toc);

