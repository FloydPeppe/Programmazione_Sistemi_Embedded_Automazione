function x_filtered = dscrtLowpass(x,alpha)
% Filtro passa basso, pesa il vettore al passo corrente con quello al precedente. 
% Inserisci matrici 3x2 in ingresso, sulle righe scorrono le componenti,
% sulle colonne il tempo.

if size(x,1) ~= 3
    disp('Dimensione errata! Inserisci matrici 3x2')
    return
end

if ~isnan(x) 
    x_filtered= alpha*x(:,2) + (1-alpha)*x(:,1);
else
    x_filtered= x(:,2);
end

end