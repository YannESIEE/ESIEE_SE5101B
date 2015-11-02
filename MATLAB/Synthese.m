%Période d'échantionnage en millisecondes
Tech=10;

% Linéarisation
[A,B,C,D]=linmod('pendule1',[0,0,0,0]');	

% Determination de l'état
[size_p,X0_p,Xstring_p]=pendule1;

%Modèle discrétisé
[Ad,Bd]=c2d(A,B,Tech/1000);    
Cd=C;
Dd=D;

% Choix des pôles du contrôleur continu
Pc=[-4,-8.5,-3,-2.5];

% Choix des pôles du contrôleur discret
Pcd=exp(Pc*Tech/1000);

% Calcul les gains de retour dans le cas sans biais 
Kcd=place(Ad,Bd,Pcd);

% Calcul les gains de retour dans le cas avec biais 
%Kcd=[Kcd 1];                    

% Pole "correspondant" à l'observation du biais
%P_p=-2;

% Dynamique de l'observateur 5 fois plus rapide que celle du controleur
K_o=5;                      

% Choix des pôles de l'observateur continu
Po=K_o*Pc;

% Choix des pôles de l'observateur discret
Pod=exp(Po*Tech/1000);

% Modèle augmenté contiu
% Ab=[A B;0 0 0 0 0];
% Bb=[B;0];
% Cb=[C,[0,0]'];
% Db=D;

% Modèle augmenté discret
% [Adb,Bdb]=c2d(Ab,Bb,Tech/1000);
% Cdb=Cb;
% Ddb=Db;

% Calcul de gain de retour de l'observateur
Kod=place(Ad',Cd',Pod);
Kod=Kod';

%Modèle d'état de l'observateur-controleur
[Adc,Bdc,Cdc,Ddc]=reg(Ad,Bd,Cd,Dd,Kcd,Kod);

