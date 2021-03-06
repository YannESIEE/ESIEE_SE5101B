/*
 * Cours : 			SE-5101-A & SE-5101-B
 * Professeurs : 	R. Kocik
 * 					R. Hamouche
 * Etude de cas : 	Réalisation de l'asservisement d'un pendule sur un calculateur déporter via communication CAN.
 * Description : 	Ce programme est capable d'asservire un ou deux pendules via une communication CAN. 
 * 					dans le cas de l'asservissement de deux pendules, une tache sera asigné a l'actionneur 
 * 					et capteur du pendule, et une autre tache correspondera au traitement du pendule déporté.
 */
/**************************************************************************\
|********************************* - TEST - *******************************|
\**************************************************************************/
#define TEST 				0 			// Permet de simuler des valeurs pour l'envoi sur le bus CAN
#define TEST_ANGLE_NUM_IN 	3072 		// Valeur simulé à envoyer dans le bus CAN pour l'angle.
#define TEST_POS_NUM_IN 	3072 		// Valeur simulé à envoyer dans le bus CAN pour la position linéaire.
#define TEST_COMMANDE_OUT 	3072 		// Valeur simulé à envoyer dans le bus CAN pour la commande calculé.
#define DEBUG_AFF_ROUTINE 	0 			// Affichage de la routine (niveaux 0, 1, 2, 3)
#define DEBUG_AFF_INTERUPT 	0 			// Affichage de l'interuption du bus CAN (niveaux 0, 1 ,2 ,3)
#define DEBUG_AFF_MAT 		0 			// Affichage du traitement (niveaux 0, 1, 2, 3)
/**************************************************************************\
|********************* - define pour tache periodique - *******************|
\**************************************************************************/
#define STACK_SIZE			2000
#define TICK_PERIOD			1000000		//1 ms
#define PERIODE_CONTROL		10000000	//20ms
#define N_BOUCLE			10000000
#define NUMERO				1
#define PRIORITE			1
/**************************************************************************\
|************************** - INFO SUR LES BANC - *************************|
\**************************************************************************/
#define MODE_PENDULE 		1								// 1 ou 2 selon un ou deux pendules inverse
#define MAX_ANGL        	(35.0/2.0*3.14159265359/180.0) 	// Amplitude totale du pendule. (35° = 0.61 rad)
#define MAX_POS            	0.6 							// 0.60 m
#define CHANNEL_ANGL        0 								// Channel de l'acquisition angulaire.
#define CHANNEL_POS       	1 								// Channel de l'acquisition de position linéaire.
#define BANC        		5       						// Banc sur lequel l'ARCOM fonctionne BANC = [2; 3; 4; 5] /!\ choisir le banc déporter si MODE_PENDULE == 2 /!\
#if (BANC == 2)
	#define BC_A0 		2007
    #define BC_PMn		367
    #define BC_PMx		3839
    #define BC_AMn		1100
    #define BC_AMx		2900
#elif (BANC == 3)
	#define BC_A0 		2040 	// Verifier
    #define BC_PMn		400 	//3
    #define BC_PMx		3900 	//4094
    #define BC_AMn		1200 	//1217
    #define BC_AMx		2800 	//2819
#elif (BANC == 4)
	#define BC_A0 		2048	// verif nico par oscilo le 02/11/ 2022
    #define BC_PMn		322		// ATTENTION PAS CENTREE ! : -8.43V et +8.71V
    #define BC_PMx		3832
    #define BC_AMn		1302 	// +3.64V et -3.64V
    #define BC_AMx		2793
#elif (BANC == 5)
	#define BC_A0 		2040    // 2007 : verif nico par oscilo le 03/11 || le pendule penche toujours dans le meme sens ! (2050 pour essayer de le recalibrer) 
    #define BC_PMn		594
    #define BC_PMx		4073
    #define BC_AMn		1247
    #define BC_AMx		2828
#else
	#define BC_A0 		2048
    #define BC_PMn		380
    #define BC_PMx		3820
    #define BC_AMn		1200
    #define BC_AMx		2800
#endif

/**************************************************************************\
|******************************* - MATRICES - *****************************|
\**************************************************************************/
#if MODE_PENDULE == 1
/*Matrice un seul pendule */
/* //Matrice de CELA selon SaGlamPourMoi :
Adc[0][0]= 0.6300;    Adc[0][1]=-0.1206;    Adc[0][2]=-0.0008;    Adc[0][3]= 0.0086;
Adc[1][0]=-0.0953;    Adc[1][1]= 0.6935;    Adc[1][2]= 0.0107;    Adc[1][3]= 0.0012;
Adc[2][0]=-0.2896;    Adc[2][1]=-1.9184;    Adc[2][2]= 1.1306;    Adc[2][3]= 0.2351;
Adc[3][0]=-3.9680;    Adc[3][1]=-1.7733;    Adc[3][2]=-0.1546;    Adc[3][3]= 0.7222;
Bdc[0][0]= 0.3658;    Bdc[0][1]= 0.1200;
Bdc[1][0]= 0.0993;    Bdc[1][1]= 0.3070;
Bdc[2][0]= 1.0887;    Bdc[2][1]= 2.0141;
Bdc[3][0]= 3.1377;    Bdc[3][1]= 1.6599;
Cdc[0]=-80.3092;    Cdc[1]=-9.6237;    Cdc[2]=-14.1215;    Cdc[3]=-23.6260;
*/

/*//Matrice de SaGlamPourMoi : (a l air pas mal dapres moi (nico))
Adc[0][0]= 0.6601;    Adc[0][1]= 0.0680;    Adc[0][2]=-0.0008;    Adc[0][3]= 0.0086;
Adc[1][0]= 0.0662;    Adc[1][1]= 0.7861;    Adc[1][2]= 0.0107;    Adc[1][3]= 0.0012;
Adc[2][0]= 1.3274;    Adc[2][1]=-0.8919;    Adc[2][2]= 1.1306;    Adc[2][3]= 0.2351;
Adc[3][0]=-3.1767;    Adc[3][1]= 0.4320;    Adc[3][2]=-0.1546;    Adc[3][3]= 0.7222;
Bdc[0][0]= 0.3358;    Bdc[0][1]=-0.0686;
Bdc[1][0]=-0.0622;    Bdc[1][1]= 0.2144;
Bdc[2][0]=-0.5283;    Bdc[2][1]= 0.9876;
Bdc[3][0]= 2.3464;    Bdc[3][1]=-0.5454;
Cdc[0]=-80.3092;    Cdc[1]=-9.6237;    Cdc[2]=-14.1215;    Cdc[3]=-23.6260;
*/
	
#define ADC_0_0  0.6196
#define ADC_1_0  0.0971
#define ADC_2_0  1.8124
#define ADC_3_0 -3.8837
#define ADC_0_1  0.0968
#define ADC_1_1  0.7038
#define ADC_2_1 -1.7997
#define ADC_3_1  0.8724
#define ADC_0_2 -0.0008
#define ADC_1_2  0.0107
#define ADC_2_2  1.1306
#define ADC_3_2 -0.1546
#define ADC_0_3  0.0086
#define ADC_1_3  0.0012
#define ADC_2_3  0.2351
#define ADC_3_3  0.7222
#define BDC_0_0  0.3762
#define BDC_1_0 -0.0931
#define BDC_2_0 -1.0133
#define BDC_3_0  3.0534
#define BDC_0_1 -0.0973
#define BDC_1_1  0.2966
#define BDC_2_1  1.8954
#define BDC_3_1 -0.9858
#define CDC_0 -80.3092
#define CDC_1 -9.6237
#define CDC_2 -14.1215
#define CDC_3 -23.6260
	
/*PERIODE_CONTROL = 10 000 000 -> 10 ms 
 * Tech = 10ms
 * P = [ -4, -8.5, -3, -2.5] */
/*Adc[0][0]= 0.6200;    Adc[0][1]= 0.0816;    Adc[0][2]=-0.0013;    Adc[0][3]= 0.0080;
Adc[1][0]= 0.0939;    Adc[1][1]= 0.5994;    Adc[1][2]= 0.0111;    Adc[1][3]= 0.0017;
Adc[2][0]= 2.3148;    Adc[2][1]=-3.1690;    Adc[2][2]= 1.1223;    Adc[2][3]= 0.3316;
Adc[3][0]=-4.4627;    Adc[3][1]= 0.6851;    Adc[3][2]=-0.2633;    Adc[3][3]= 0.6079;
Bdc[0][0]= 0.3739;    Bdc[0][1]=-0.0828;
Bdc[1][0]=-0.0882;    Bdc[1][1]= 0.4016;
Bdc[2][0]=-1.1846;    Bdc[2][1]= 3.3700;
Bdc[3][0]= 3.2403;    Bdc[3][1]=-0.9231;
Cdc[0]=-113.5831;    Cdc[1]=-20.1981;    Cdc[2]=-23.3416;    Cdc[3]=-33.3277;
*/

#elif MODE_PENDULE == 2
	/*Matrice deux pendules (à vérifier) (NE MARCHE PAS POUR PENDULE SEUL) */
#define ADC_0_0  0.6300
#define ADC_1_0 -0.0953
#define ADC_2_0 -0.2896
#define ADC_3_0 -3.9680
#define ADC_0_1 -0.1206
#define ADC_1_1  0.6935
#define ADC_2_1 -1.9184
#define ADC_3_1 -1.7733
#define ADC_0_2 -0.0008
#define ADC_1_2  0.0107
#define ADC_2_2  1.1306
#define ADC_3_2 -0.1546
#define ADC_0_3  0.0086
#define ADC_1_3  0.0012
#define ADC_2_3  0.2351
#define ADC_3_3  0.7222
#define BDC_0_0  0.3658
#define BDC_1_0  0.0993
#define BDC_2_0  1.0887
#define BDC_3_0  3.1377
#define BDC_0_1  0.1200
#define BDC_1_1  0.3070
#define BDC_2_1  2.0141
#define BDC_3_1  1.6599
#define CDC_0 -80.3092
#define CDC_1 -9.6237
#define CDC_2 -14.1215
#define CDC_3 -23.6260
#endif


/**************************************************************************\
|************************* - MACRO DE CONVERSION - ************************|
\**************************************************************************/
#define POS_CONVERT(x)	(((int)x - (BC_PMx-BC_PMn)/2.0) * MAX_POS*2.0 / (BC_PMx-BC_PMn))	// Conversion position linéaire
#define ANG_CONVERT(x)	(((int)x - BC_A0 ) * MAX_ANGL*2.0 / (BC_AMx-BC_AMn))				// Conversion position angulaire

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>

/* AJOUT DES LIBRAIRES */
#include "ADC.c"
#include "DAC.c"
#include "CAN.c"

MODULE_LICENSE("GPL");

/**************************************************************************\
|***************** - Declaration des variables globales - *****************|
\**************************************************************************/
/* RT_TASK */

#if MODE_PENDULE == 1
static RT_TASK tache_one;
#elif MODE_PENDULE == 2
static RT_TASK tache_in, tache_out;
int glb_task_in_wait;
int glb_task_out_wait;
#endif
unsigned int command_in, angle_num_out, pos_num_out; // information communiqué par bus CAN
/* Matrices */
/*float Adc[4][4];
float Bdc[4][2];
float Cdc[4];*/
float x[4];
float x_save[4];
float y[2];
unsigned int didit;

/**************************************************************************\
|************************ - PROTYPE DES FONCTIONS - ***********************|
\**************************************************************************/
/* LANCEMENT DU PROGRAMME */
static int 	prog_init(void);
static void prog_exit(void);
/* ROUTINE */
#if MODE_PENDULE == 2
void			task_in 			(long arg);
void			task_out 			(long arg);
static void 	routine_reception 	(void);
#elif MODE_PENDULE == 1
void			task_one 			(long arg);
#endif
/* MATRICE */
void  	init_matrix 	(void);
float 	calc_matrix 	(float *y, float *x);
void 	affichage_float (float val);

/**************************************************************************\
|*********************** - LANCEMENT DU PROGRAMME - ***********************|
\**************************************************************************/
module_init(prog_init);
module_exit(prog_exit);

/*
 * prog_init
 * Fonction de lancement des taches et des interuption.
 * @return : 1 succes de l'opération
 */
static int prog_init(void)
{
#if MODE_PENDULE == 2
	int ierr_in;
	int ierr_out;
#elif MODE_PENDULE == 1
	int ierr_one;
#endif
	RTIME now;

	printk("\n\n\n\nprog_init : Programme Initialisation...\n");
	rt_set_oneshot_mode();

	start_rt_timer(nano2count(TICK_PERIOD));
	now = rt_get_time();
	/* AFFICHAGE D'INFORMATION SUR LE BANC */
	printk("prog_init :\n\tBanc : %d\n\tBC_PMn = %d\n\tBC_PMx = %d\n\tBC_AMn = %d\n\tBC_AMx = %d\n", BANC, BC_PMn, BC_PMx, BC_AMn, BC_AMx );

	init_matrix();		// initialisation des matrices
	init_3718();		// init de l'adc
#if MODE_PENDULE == 2
	init_can();			// Initialisation du bus CAN
	/* Tache d'acquisition/action */
	ierr_in = rt_task_init(&tache_in,task_in,0,STACK_SIZE, PRIORITE, 1, 0);
	rt_task_make_periodic(&tache_in, now, nano2count(PERIODE_CONTROL));
	/* Tache de traitement déporter */
	ierr_out = rt_task_init(&tache_out,task_out,0,STACK_SIZE, PRIORITE, 1, 0);
	rt_task_make_periodic(&tache_out, now, nano2count(PERIODE_CONTROL));
	/* Interuption de lecture de donnée. */
	rt_global_cli();									// Desactivation des IT
	rt_request_global_irq(5,routine_reception);			// Association de l interruption avec la routine
	rt_startup_irq(5);									// Activation de la ligne d interruption
	rt_global_sti();									// Re-activation des IT
#elif MODE_PENDULE == 1
	/* Tache d'asservissement sans communication CAN */
	ierr_one = rt_task_init(&tache_one,task_one,0,STACK_SIZE, PRIORITE, 1, 0);
	rt_task_make_periodic(&tache_one, now, nano2count(PERIODE_CONTROL));
#endif
	printk("prog_init : exiting init\n");
	return(0);
}

/*
 * prog_exit
 * Fonction d'arret des taches et des interuptions
 */
static void prog_exit(void)
{
	printk("prog_exit : stoping prog\n");
#if MODE_PENDULE == 2
	rt_shutdown_irq(5);			// Desactivation de l IT
	rt_free_global_irq(5);		// Desinstallation du handler
#endif
	stop_rt_timer();			// Arrèt de l'ordonnanceur
	/* Suppression des taches */
#if MODE_PENDULE == 2
	rt_task_delete(&tache_in);
	rt_task_delete(&tache_out);
#elif MODE_PENDULE == 1
	rt_task_delete(&tache_one);
#endif
	printk("prog_exit : EXIT\n");
}

/**************************************************************************\
|********************** - Declaration des routines - **********************|
\**************************************************************************/

#if MODE_PENDULE == 2
/*
 * task_in
 * Cette routine gère l'acquisition A-N du banc (in)
 * Transmet l'information via le bus CAN
 * attend la reception de la donnée, puis génère la tension de la commande reçut
 */
void task_in(long arg)
{
#if DEBUG_AFF_ROUTINE >= 1
	printk("task_in : init\n");
#endif
	/* Decalaration et initialisaiton des variables. */
	u8 data_send_in[4];
	unsigned int adc_value, angle_num_in=0, pos_num_in=0;
	glb_task_in_wait = 1;
	command_in = 0;
	while(1)
	{
		/* Acquisition (2 acquisition: Angle/Position) */
#if TEST
		angle_num_in 	= TEST_ANGLE_NUM_IN;
		pos_num_in		= TEST_POS_NUM_IN;
		adc_value 		= 0;				// uniquement pour eviter un warning
#else
		trigger();
		while(adc_read_eoc() != 1);			// Attente de fin d'acquisition
		adc_value = adc_read_value();		// recuperation du (canal + valeur) concaténés
		if 		((adc_value & 0x0F) == 1){pos_num_in 	= adc_value >> 4 ;}
		else if ((adc_value & 0x0F) == 0){angle_num_in 	= adc_value >> 4 ;}
		trigger();
		while(adc_read_eoc() != 1);			// Attente de fin d'acquisition
		adc_value = adc_read_value();		// recuperation du (canal + valeur) concaténés
		if 		((adc_value & 0x0F) == 1){pos_num_in 	= adc_value >> 4 ;}
		else if ((adc_value & 0x0F) == 0){angle_num_in 	= adc_value >> 4 ;}
#endif
		/* Envoi des données */
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_in : send angle_num_in = %d & pos_num_in = %d\n",angle_num_in, pos_num_in);
#endif
		do
		{
#if DEBUG_AFF_ROUTINE >= 3
			printk("task_in : retry to send\n");
#endif
			data_send_in[0] = ((angle_num_in >> 8) & 0x0F) + CAN_COMM_ACQUISITION;	// Envoi d'un entête de 4 bits. envoi des 4 bits de poids fort de angle_num_in
			data_send_in[1] = angle_num_in & 0xFF;									// Envoi des bits de poids faible de angle_num_in
			data_send_in[2] = (pos_num_in >> 8) & 0x0F; 							// Envoi des bits de poids fort de pos_num_in
			data_send_in[3] = pos_num_in & 0xFF; 									// Envoi des bits de poids faible de pos_num_in
			emission(CAN_SEND_ID,data_send_in, 4, 0);
			/* Swich de routine en attendant la réponse.... */
			rt_task_wait_period();
			/* Attente de reception */
		}
		while(glb_task_in_wait);
		glb_task_in_wait = 1; 	// Réinitialise la variable glb_task_in_wait (variable de confirmation de reception de la trame contenant la commande.)
		/* Actionneur */
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_in : received command_in = %d\n",command_in);
#endif
		set_DAC(0, command_in);	// Envoi de la commande reçu sur le DAC
	}
}

/*
 * task_out
 * Cette routine gère le traitement du banc déporter (out)
 * Reception de donné de position/angle (status)
 * Traitement
 * Envoi de la commande
 */
void task_out(long arg)
{
#if DEBUG_AFF_ROUTINE >= 1
	printk("task_out : init\n");
#endif
	u8 data_send_out[2];
	int command_out;
	glb_task_out_wait = 1;
	pos_num_out = 2048;
	angle_num_out = 2048;
	float y[2] = {0,0};
	float x[4] = {0,0,0,0};	
	while(1)
	{
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_out : received angle_num_out = %d & pos_num_out = %d\n",angle_num_out, pos_num_out);
#endif
		/* Conversion */
		y[0] = ANG_CONVERT(angle_num_out);
	    y[1] = POS_CONVERT(pos_num_out);
		/* Traitement */
#if TEST
		command_out = TEST_COMMANDE_OUT;
#else
		command_out = (int)((calc_matrix(y,x) + 10.0) * 4095.0 / 20.0); // Convertion Volt/numerique
		command_out = command_out<0 ? 0 : command_out>4095 ? 4095 : command_out;
#endif
		/* renvoi des donnée */
		data_send_out[0] = ((command_out >> 8) & 0x0F) + CAN_COMM_COMMAND;
		data_send_out[1] = command_out & 0xFF;
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_out : send command_out = %d\n",command_out);
#endif
		do
		{
#if DEBUG_AFF_ROUTINE >= 3
			printk("task_out : retry to send\n");
#endif
			emission(CAN_SEND_ID,data_send_out, 2, 0);
			/* SWITCH ROUTINE */
			rt_task_wait_period();
		}
		/* Attende de reception */
		while(glb_task_out_wait);
		glb_task_out_wait = 1;

	}
}

#elif MODE_PENDULE == 1
/*
 * task_one 
 * routine de gestion d'un pendule.
 * Pas de communication CAN
 */
void task_one(long arg)
{
#if DEBUG_AFF_ROUTINE >= 1
	printk("task_one : init\n");
#endif
	unsigned int adc_value, angle_num_one=0, pos_num_one=0;
	int command_one;
	float y[2] = {0,0};
	float x[4] = {0,0,0,0};
	while(1)
	{
		/* ACQUISITION */
		trigger();
		while(adc_read_eoc() != 1);			// Attente de fin d'acquisition
		adc_value = adc_read_value();		// recuperation du (canal + valeur) concaténés
		if 		((adc_value & 0x0F) == 1){pos_num_one 	= adc_value >> 4 ;}
		else if ((adc_value & 0x0F) == 0){angle_num_one	= adc_value >> 4 ;}
		trigger();
		while(adc_read_eoc() != 1);			// Attente de fin d'acquisition
		adc_value = adc_read_value();		// recuperation du (canal + valeur) concaténés
		if 		((adc_value & 0x0F) == 1){pos_num_one 	= adc_value >> 4 ;}
		else if ((adc_value & 0x0F) == 0){angle_num_one	= adc_value >> 4 ;}
		y[0] = ANG_CONVERT(angle_num_one);//angle_num_one
	    y[1] = POS_CONVERT(pos_num_one);
		/* TRAITEMENT */
		command_one = (int)((calc_matrix(y,x) + 10.0) * 4095.0 / 20.0); 					// Convertion Volt/numerique
		command_one = command_one<0 ? 0 : command_one>4095 ? 4095 : command_one;			// Borne: [0:4095] en numérique (-10:+10 V)
		comande_one = y[1]>0.5 || y[1]<-0.5 ? 2048 : command_one;							// vérification de bordure.
#if DEBUG_AFF_ROUTINE == 3 // Affichage Angle & position trop important pour ARCOM -> Crache : Affichage angle unique
		/** Affichages des angles **/
		printk("task_one :\n\tangle_num =\t%d\n\tangle_rad =\t",angle_num_one,pos_num_one);
		affichage_float(y[0]);
		printk("\n");
#elif DEBUG_AFF_ROUTINE == 4 // Affichage Angle & position trop important pour ARCOM -> Crache : Affichage position unique
		/** Affichages de position **/
		printk("task_one :\n\tpos_num =\t%d\n\tpos_metre =\t",pos_num_one);
		affichage_float(y[1]);
		printk("\n");
#endif
#if DEBUG_AFF_ROUTINE >= 2
		/** affichage de la commande **/
		printk("task_one :\n\tcommande_num = %d\n", command_one);
#endif
		/* ACTIONNEUR */
		set_DAC(0, command_one);	// on ecrit dans le canal 0, la "commande"
		rt_task_wait_period();
	}
}
#endif

#if MODE_PENDULE == 2
/*
 * routine_reception
 * Interuption de reception de donnée.
 */
static void routine_reception(void)
{

#if DEBUG_AFF_INTERUPT >= 1
	printk("routine_reception : trame received\n");
#endif
	unsigned int value[2];
	reception(CAN_FOCUS_ID, value);	// Lecture

#if DEBUG_AFF_INTERUPT >= 3
	printk("routine_reception :\n\tvalue[0] = 0x%x\n\tvalue[1] = 0x%x\n\tcomm : 0x%x\n",value[0],value[1], (value[0]>>8));
#endif
	/* repartition selon  */
	if(((value[0]>>8) & 0xF0) == CAN_COMM_COMMAND)
	{
		command_in = value[0] & 0x0FFF;
		glb_task_in_wait = 0;
#if DEBUG_AFF_INTERUPT >= 2
		printk("routine_reception : received command\n");
#endif
	}
	else if(((value[0]>>8) & 0xF0) == CAN_COMM_ACQUISITION)	
	{
		angle_num_out 	= value[0] & 0x0FFF;
		pos_num_out 	= value[1];
		glb_task_out_wait = 0;
#if DEBUG_AFF_INTERUPT >= 2
		printk("routine_reception : received data\n");
#endif
	}

	inb(CAN_INTERRUPT);		// Lecture du registre d interruptions pour liberer celle traitee
	rt_ack_irq(5);			// Acquitement de l interruption au niveau du CPU
#if DEBUG_AFF_INTERUPT >= 1
	printk("routine_reception : trame traited\n");
#endif
	return;
}
#endif


/**************************************************************************\
|***************************** - LIB MATRIX - *****************************|
\**************************************************************************/
/*
 * init_matrix
 * Fonction d'initialisation des matrices en fonction du mode
 */
void init_matrix(void)
{
#if DEBUG_AFF_MAT >= 1
	printk("init_matrix : begin initialization\n");
#endif

// ex init Adc, Bdc, Cdc

	y[0]=0;
	y[1]=0;

	x[0]=0;
	x[1]=0;
	x[2]=0;
	x[3]=0;
#if DEBUG_AFF_MAT >= 1
	printk("init_matrix : end initialization\n");
#endif
}

/*
 * Fonction de calcule de la commande avec les matrice d'observation et la matrice d'état.
 */
float calc_matrix(float *y, float *x)
{
	/* Declaration de variable */
	float commande; // commande : varaible de sortie
	float x_plus_1[4];
#if DEBUG_AFF_MAT >= 1
	printk("calc_matrix:\n"); // Affichage de debugage niveau 1.
#endif
	/* CALCULE DE LA COMMANDE */
	/* x = {	y[0], 	y[1],	vit_ang,	vit_pos } */
	/** CALCULE DE LA MATRICE D'ETAT : X = Adc * X + Bdc * Y **/
	x_plus_1[0]= ADC_0_0*x[0] + ADC_0_1*x[1] + ADC_0_2*x[2] + ADC_0_3*x[3] + BDC_0_0*y[0] + BDC_0_1*y[1];
	x_plus_1[1]= ADC_1_0*x[0] + ADC_1_1*x[1] + ADC_1_2*x[2] + ADC_1_3*x[3] + BDC_1_0*y[0] + BDC_1_1*y[1];
	x_plus_1[2]= ADC_2_0*x[0] + ADC_2_1*x[1] + ADC_2_2*x[2] + ADC_2_3*x[3] + BDC_2_0*y[0] + BDC_2_1*y[1];
	x_plus_1[3]= ADC_3_0*x[0] + ADC_3_1*x[1] + ADC_3_2*x[2] + ADC_3_3*x[3] + BDC_3_0*y[0] + BDC_3_1*y[1];
	/** CALCULE DE LA COMMANDE commande = u = -Cdc * X **/
	commande =  + CDC_0*(x_plus_1[0]) + CDC_1*(x_plus_1[1]) + CDC_2*(x_plus_1[2]) + CDC_3*(x_plus_1[3]);
#if DEBUG_AFF_MAT >= 2 // Affichage de debugage niveau 2
	printk("calc_matrix :\n\tx =\t\t");affichage_float(x[0]);printk(" ; ");affichage_float(x[1]);printk(" ; ");affichage_float(x[2]);printk(" ; ");affichage_float(x[3]);
	printk("\n\tx+1 =\t\t");affichage_float(x_plus_1[0]);printk(" ; ");affichage_float(x_plus_1[1]);printk(" ; ");affichage_float(x_plus_1[2]);printk(" ; ");affichage_float(x_plus_1[3]);
	printk("\n\ty =\t\t");affichage_float(y[0]);printk(" ; ");affichage_float(y[1]);
	printk("\n\tcommande = \t"); affichage_float(commande);printk("\n");
#endif
	/** MEMORISATION DE LA MATRICE D'ETATS ACTUEL **/
	x[0]=(x_plus_1[0]);
	x[1]=(x_plus_1[1]);
	x[2]=(x_plus_1[2]);
	x[3]=(x_plus_1[3]);
	/** GESTION EXTREMUM **/
	return commande;
}

/*
 * affichage_float
 * Si float corrompu : -210000000,00-2100000;
 */
void affichage_float(float val)
{
	if(val>=0)
	{
		int decimal = (int)(val*1000)-((int)val)*1000;
		if		(decimal>100)	printk("%d,%d", 	(int)val,	(int)(val*1000)-((int)val)*1000 );
		else if	(decimal > 10 )	printk("%d,0%d", 	(int)val,	(int)(val*1000)-((int)val)*1000 );
		else					printk("%d,00%d", 	(int)val,	(int)(val*1000)-((int)val)*1000 );
		
	}
	else
	{
		int decimal = (int)((-val)*1000)-((int)(-val))*1000;
		if		(decimal>100)	printk("-%d,%d", 	(int)(-val),	(int)((-val)*1000)-((int)(-val))*1000 );
		else if	(decimal > 10 )	printk("-%d,0%d", 	(int)(-val),	(int)((-val)*1000)-((int)(-val))*1000 );
		else					printk("-%d,00%d", 	(int)(-val),	(int)((-val)*1000)-((int)(-val))*1000 );
		
	}
}
