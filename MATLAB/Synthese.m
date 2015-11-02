%P�riode d'�chantionnage en millisecondes
Tech=10;

% Lin�arisation
[A,B,C,D]=linmod('pendule1',[0,0,0,0]');	

% Determination de l'�tat
[size_p,X0_p,Xstring_p]=pendule1;

%Mod�le discr�tis�
[Ad,Bd]=c2d(A,B,Tech/1000);    
Cd=C;
Dd=D;

% Choix des p�les du contr�leur continu
Pc=[-4,-8.5,-3,-2.5];

% Choix des p�les du contr�leur discret
Pcd=exp(Pc*Tech/1000);

% Calcul les gains de retour dans le cas sans biais 
Kcd=place(Ad,Bd,Pcd);

% Calcul les gains de retour dans le cas avec biais 
%Kcd=[Kcd 1];                    

% Pole "correspondant" � l'observation du biais
%P_p=-2;

% Dynamique de l'observateur 5 fois plus rapide que celle du controleur
K_o=5;                      

% Choix des p�les de l'observateur continu
Po=K_o*Pc;

% Choix des p�les de l'observateur discret
Pod=exp(Po*Tech/1000);

% Mod�le augment� contiu
% Ab=[A B;0 0 0 0 0];
% Bb=[B;0];
% Cb=[C,[0,0]'];
% Db=D;

% Mod�le augment� discret
% [Adb,Bdb]=c2d(Ab,Bb,Tech/1000);
% Cdb=Cb;
% Ddb=Db;

% Calcul de gain de retour de l'observateur
Kod=place(Ad',Cd',Pod);
Kod=Kod';

%Mod�le d'�tat de l'observateur-controleur
[Adc,Bdc,Cdc,Ddc]=reg(Ad,Bd,Cd,Dd,Kcd,Kod);

