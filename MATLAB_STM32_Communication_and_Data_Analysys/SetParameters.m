clear
clc

% Script in cui è possibile impostare i parametri
% fisici del differential drive


%% Parametri fisici 

% ------ Mechanical parameters 

prmtrs.WHEEL_DISTANCE.val=  0.155/2.0;  % Distance between wheels in meters.
prmtrs.WHEEL_RADIUS.val=    0.069/2.0;  % Wheels radius in meters.

prmtrs.WHEEL_MAX_SPEED.val= 10.0;  % Radiant each seconds (rad/s). DC motors and gears specifics,
						     	  % on about the working voltage (~ 6-7V), is 160 rpm (~ 16 rad/s), with
							     % no load, so here I use a smaller value. Below, the web link of the
							    % motor specifics:
						       %
  						      % https://wiki.dfrobot.com/Micro_DC_Motor_with_Encoder-SJ02_SKU__FIT0458
						     %

% ------ Voltage constants 

prmtrs.SUPPLY_VOLTAGE.val=     7.5;
prmtrs.MOTOR_MIN_VOLTAGE.val=  0.0;
prmtrs.MOTOR_MAX_VOLTAGE.val=  prmtrs.SUPPLY_VOLTAGE.val;


% ---- Time parameters

prmtrs.TIME_STEP.val= 0.01; % Control time step

% Comments to add on C file generation
prmtrs.WHEEL_DISTANCE.comment= " Half the distance between wheels, in meters";
prmtrs.WHEEL_RADIUS.comment=   " Wheels radius in meters";
prmtrs.WHEEL_MAX_SPEED.comment= " Radiant each seconds (rad/s). DC motors and gears specifics, on\n" + ...
							    " about the working voltage (~ 6-7V), is 160 rpm (~ 16 rad/s), with\n" + ...
							    " no load, so here I use a smaller value. Below, the web link of the\n" + ...
                                " motor specifics:\n\n" + ...
                                " https://wiki.dfrobot.com/Micro_DC_Motor_with_Encoder-SJ02_SKU__FIT0458";

prmtrs.SUPPLY_VOLTAGE.comment=    " Supply Voltage ";
prmtrs.MOTOR_MIN_VOLTAGE.comment= " Minimum voltage ";
prmtrs.MOTOR_MAX_VOLTAGE.comment= " Maximum voltage ";
prmtrs.TIME_STEP.comment= " Control law time step";

save('Exports/parameters','prmtrs')

clear
