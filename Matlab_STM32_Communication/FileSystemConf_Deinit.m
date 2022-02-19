
function FileSystemConf_Deinit
%% Elimino i percorsi e riconfiguro la cartella cache di default 

% Removing Paths
rmpath('SimulinkModel','ScriptUtili');

% Default Cache Folder
cfg= Simulink.fileGenControl('getConfig');
cfg.CacheFolder = '';
Simulink.fileGenControl('setConfig', 'config', cfg)

end