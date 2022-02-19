
function FileSystemConf_Init
%% Aggiungo le cartelle secondarie e imposto la cartella per le cache

% Percorsi 
addpath('SimulinkModel','ScriptUtili');

% Cartella Cache Simulink
cfg= Simulink.fileGenControl('getConfig');
cfg.CacheFolder = fullfile(eval('pwd'),'generatedCache');
Simulink.fileGenControl('setConfig', 'config', cfg,'createDir',true);

% A volte aggiunge anche la cartella corrente e generatedCache al percorso

% ====== In caso li elimino ==============================================
pathCell = regexp(path, pathsep, 'split');
if sum(strcmpi(pathCell,pwd))
    rmpath('./') 
end

pathCell = regexp(path, pathsep, 'split');
if sum(strcmpi(pathCell,strcat(pwd,'/generatedCache')))
    rmpath('generatedCache') 
end
% ========================================================================

end