%% Resetting MATLAB environment
instrreset; clear ; close all; clc
addpath('Phone functions','ScriptUtili');

%% general setting
N_sample        = 10000; % how many samples to collect

%% create phone listener object: Android
%IpAdress      = '160'; 
IpAdress      = '192';
phonelistener   = makeandroidlistener(IpAdress);

%% Initialise empty figure and empty variables
%{
fig= figure(1);
h_plot  = plot(NaN,NaN);
t       = NaN*ones(1,N_sample);
acc     = NaN*ones(3,N_sample);
acc_ned = NaN*ones(3,N_sample); 
theta   = NaN*ones(1,N_sample);
phi     = NaN*ones(1,N_sample);

Delta= 5; %% Lato del quadrato
ax= gca(fig); 
axis(ax,'equal',[-1.4 1.4 -1.4 1.4]*Delta); grid on
line(ax,[-1 1 1 -1 -1]*1.1*Delta,[-1 -1 1 1 -1]*1.1*Delta,'color','r')
%}

t       = NaN*ones(1,N_sample);
acc     = NaN*ones(3,N_sample);
acc_ned = NaN*ones(3,N_sample); 
theta   = NaN*ones(1,N_sample);
phi     = NaN*ones(1,N_sample);
om      = NaN*ones(3,N_sample);
xh_k    = NaN*ones(2,N_sample);

t1= tic; 

% ==================== Serial Microcontroller Connection ================


% Hardware info
hw= instrhwinfo('serial');

% Opening file descriptor for the serial connection
fd_s= serial(hw.AvailableSerialPorts{1},'BaudRate',115200);
fopen(fd_s);

% Time vector
%Fs= 1e2;
%t= 0:1/Fs:60;


Delta_deg= 0;
deg= 0;
theta_prev= 0;
theta_dot_prev= 0;
dt= 1e-2;
PID= 0;

Ad = [ 1.0000, 0.0500   
            0, 1.0000 ];
        
Bd = [12.8384; 513.5371];

Cd = [1.0000 0];

Kd= [-0.0056  -0.0013];

Ld = [2.0000; 19.8000];

xh_prev= [0; 0];
yh_prev= Cd*xh_prev;
y_prev= yh_prev;

%%% Loop to start data collection
for k = 1:N_sample
    
    tmp_2= tic;
    tmp= tic;
    
    for i= 1:10
        %%% Accelerazioni dello smartphone
        [t(:,k), acc(:,k), om(:,k)] = getandroiddata(phonelistener);
        if prod(~isnan(acc(1,k))) && prod(~isnan(om(1,k)))
            break
        end
    end
    
    %%% From WSD (west-south-down, sinistro-giro) to NED (north-east-down, destro-giro) 
    acc_ned(:,k)= wsd2ned(acc(:,k));

    alpha= 0.1; %% Coefficiente di taglio del filtro passa-basso (dscrtLowpass)
    if k > 1
        acc_ned(:,k)= dscrtLowpass(acc_ned(:,k-1:k),alpha);
        %om(:,k)= dscrtLowpass(om(:,k-1:k),0.9);
    end
    
    g= 9.8;
    
    %%% Trasformazione da accelerazioni in angoli di pitch e roll
    theta(k)= atan2(-acc_ned(1,k),norm(acc_ned(2:3,k)))*180/pi; %% Pitch
      phi(k)= atan2(acc_ned(2,k),acc_ned(3,k))*180/pi; %% Roll  
    
    if isnan(theta(k))
        theta(k)= theta_prev;
        1
    end
%%{    
    PID= PID + 0.07*theta(k) + 0.00/dt*(theta(k)-theta_prev);
    
    if ~isnan(om(1,k))
        PID= PID + deg2rad(om(1,k));
        theta_dot= om(1,k);
    else
        2
        theta_dot= theta_dot_prev;
    end
%}
    
 %%{   
   y= deg2rad(theta(k));
   theta_prev= theta(k);
    
   xh_k(:,k)= Ad*xh_prev + Bd*deg2rad(PID)*0.00005 + Ld*(y_prev - Cd*xh_prev);
 
   xh_prev= xh_k(:,k);
   y_prev= y;
   
   %Delta_rad= Kd*xh_k(:,k)/0.004;

   Delta_rad= Kd*[y; theta_dot]/0.004;
   Delta_deg= 0.7*Delta_deg + rad2deg(Delta_rad) - 0*PID;  
   
  %}
    
    deg= 90 - Delta_deg;

    str= compose("%7.2f",deg);
    
    fwrite(fd_s,str)
    
    dt= toc(tmp);
    
    pause(0.05-dt)

end

%% Closing and deleting connections (keep these in your program!)


fclose(phonelistener); % Closing UDP communication
delete(phonelistener); % Deleting UDP communication
fclose(fd_s);          % Closing Serial file descriptor

rmpath("Phone functions","ScriptUtili");

t= t-t(1);

subplot(2,1,1)
plot(t,[theta;rad2deg(xh_k(1,:))])
legend("Misurato","Stimato")

subplot(2,1,2)
plot(t,[om(1,:);xh_k(2,:)])
legend("Misurato","Stimato")
