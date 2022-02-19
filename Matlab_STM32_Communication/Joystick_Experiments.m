close all
clc
clear


% Plot
figure(1)
hold on
ball= scatter(NaN,NaN,'MarkerFaceColor','R');
h= plot(NaN,NaN,'LineWidth',4); %,'Marker','o');

axis([0 1040 0 1040])

x_pt= [];
y_pt= [];

% Resetting connection
instrreset;
  
% Serial info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_ser= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_ser);

pause(2)

% Start communication
fprintf(fd_ser,"\n");

x= 512;
y= 512;

ball_x= rand*1023;
ball_y= rand*1023;
set(ball,'XData',ball_x,'YData',ball_y)
cnt= 0;
title(num2str(cnt))

str_curr= fgetl(fd_ser);

val_1= 512;
val_2= 512;

tic
while true 
    
 %   flushinput(fd_ser);
    
    str= fgetl(fd_ser);
    
    val_1= str2double(str(2:5));
    val_2= str2double(str(6:9));
    
 %   val_1= val_1 + (rand-0.5)*50;
 %   val_2= val_2 + (rand-0.5)*50;
    
    if( val_1 > 1023 )
        val_1= 1023;
    end    
    if( val_2 > 1023 )
        val_2= 1023;
    end
    if( val_1 < 0 )
        val_1= 0;
    end    
    if( val_2 < 0 )
        val_2= 0;
    end
    
    alpha= 0.99;
    x= x*alpha + val_2*(1-alpha); 
    y= y*alpha + val_1*(1-alpha); 
  
    
    if( length(x_pt) > 100 + 10*cnt )
        x_pt= [x_pt(end-(99+cnt*10):end) x];
        y_pt= [y_pt(end-(99+cnt*10):end) y];
    else
        x_pt= [x_pt x];
        y_pt= [y_pt y];
    end
    %x_pt= x;
    %y_pt= y;
    
    set(h,'XData',x_pt ,'YData',y_pt)
    
    thres= 20;
    if( toc > 5 ||  norm([x-ball_x; y-ball_y]) < 20 )
        if( norm([x-ball_x; y-ball_y]) < thres )
            cnt= cnt + 1;
            title(num2str(cnt))
        end
        
        ball_x= rand*1023;
        ball_y= rand*1023;
        set(ball,'XData',ball_x,'YData',ball_y)
        
        drawnow update
        
        tic
    end
    drawnow limitrate
   
    
  %  pause(0.1);
    
end

fprintf("YOU LOOSE!\n")

fclose(fd_ser);