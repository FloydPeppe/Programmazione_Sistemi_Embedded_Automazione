function v_new= wsd2ned(v)
%%% From WSD (west-south-down, sinistro-giro) to NED (north-east-down, destro-giro) 

if size(v,1) ~= 3
    disp('Dimensione errata! Inserisci vettori 3x1 o matrici 3xN')
    return
end

%%% Lo rendo un sistema destro-giro, invertendo il senso dell'asse Y
v(2)= -v(2); 

%%% Ruoto lo yaw del sistema di 90 gradi
Om= [ 0 1 0 ; 
     -1 0 0 ; 
      0 0 1 ];
  
v_new= Om*v;

end
    