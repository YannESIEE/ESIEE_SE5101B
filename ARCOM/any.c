
/**************************************************************************\
|********************************* - TEST - *******************************|
\**************************************************************************/
#define TEST 				0
#define TEST_ANGLE_NUM_IN 	3072
#define TEST_POS_NUM_IN 	3072
#define TEST_COMMANDE_OUT 	3072
/* AFFICHAGE : Attention, il est conseillé de ne pas activé tous les affichage à la fois */
#define DEBUG_AFF_CAN 		0 // niveau trois est un peu HARD pour arcom
#define DEBUG_AFF_CAN_REC	0
#define DEBUG_AFF_DAC 		0
#define DEBUG_AFF_ROUTINE 	0
#define DEBUG_AFF_INTERUPT 	0
#define DEBUG_AFF_MAT 		0
/**************************************************************************\
|********************* - define pour tache periodique - *******************|
\**************************************************************************/
#define STACK_SIZE			2000
#define TICK_PERIOD			100000		//1 ms
#define PERIODE_CONTROL		10000000	//20ms
#define N_BOUCLE			10000000
#define NUMERO				1
#define PRIORITE			1
/**************************************************************************\
|********************************* - CAN - ********************************|
\**************************************************************************/
#define CAN_COMM_COMMAND 		0x10
#define CAN_COMM_ACQUISITION 	0x20
#define CAN_FOCUS_ID 			2
#define CAN_SEND_ID 			2
/* define pour gestion registres CAN 7841 */
#define base2					0x180
#define CAN_CONTROL 			base2
#define CAN_COMMAND 			(base2+1)
#define CAN_STATUS 				(base2+2)
#define CAN_INTERRUPT 			(base2+3)
#define CAN_ACCEPTANCE_CODE		(base2+4)
#define CAN_ACCEPTANCE_MASK		(base2+5)
#define CAN_BUS_TIMING_0		(base2+6)
#define CAN_BUS_TIMING_1		(base2+7)
#define CAN_OUTPUT_CTRL			(base2+8)
#define CAN_TEST				(base2+9)
#define CAN_TX_IDENTIFIER		(base2+10)
#define CAN_TX_RTB_BIT			(base2+11)
#define CAN_TX_BUFFER			(base2+12)
#define CAN_RX_IDENTIFIER		(base2+20)
#define CAN_RX_RTB_BIT			(base2+21)
#define CAN_RX_BUFFER			(base2+22)
/**************************************************************************\
|********************************* - ADC - ********************************|
\**************************************************************************/
#define BASE_ADC_0 0x0320
#define BASE_ADC_1 (BASE_ADC_0+1)
#define BASE_ADC_2 (BASE_ADC_0+2)
#define BASE_ADC_3 (BASE_ADC_0+3)
#define BASE_ADC_8 (BASE_ADC_0+8)
/**************************************************************************\
|********************************* - DAC - ********************************|
\**************************************************************************/
#define BASE_DAC_0 0x0300
#define BASE_DAC_1 (BASE_DAC_0+1)
#define BASE_DAC_2 (BASE_DAC_0+2)
#define BASE_DAC_3 (BASE_DAC_0+3)
#define BASE_DAC_8 (BASE_DAC_0+8)
/**************************************************************************\
|************************** - INFO SUR LES BANC - *************************|
\**************************************************************************/
#define PI 					3.14159265359
#define MODE_PENDULE 		1								// 1 ou 2 selon 1 ou deux bancs
#define MAX_ANGL        	(35.0/2.0*PI/180.0) //0.305		// (+35° = 0.61 rad)
#define MAX_POS            	0.6 							// 0.60 m
#define CHANNEL_ANGL        0
#define CHANNEL_POS       	1

// EXTREMES ARCOM
//Banc 2 à revérifier
#define BANC        0        //CHOISIR ENTRE 2,3,4,5 ou autre(défaut) **** /!\ choisir le banc déporter si MODE_PENDULE == 2 ****

#if (BANC == 2)
	#define BC_A0 		2007
    #define BC_PMn		367
    #define BC_PMx		3839
    #define BC_AMn		1100
    #define BC_AMx		2900
#elif (BANC == 3)
	#define BC_A0 		2040 // Verifier
    #define BC_PMn		400 //3
    #define BC_PMx		3900 //4094
    #define BC_AMn		1200 //1217
    #define BC_AMx		2800 //2819
#elif (BANC == 4)
	#define BC_A0 		2048
    #define BC_PMn		5
    #define BC_PMx		4095
    #define BC_AMn		1291
    #define BC_AMx		2760
#elif (BANC == 5)
	#define BC_A0 		2048
    #define BC_PMn		0
    #define BC_PMx		4095
    #define BC_AMn		1235
    #define BC_AMx		2800
#else
	#define BC_A0 		2048
    #define BC_PMn		380
    #define BC_PMx		3820
    #define BC_AMn		1200
    #define BC_AMx		2800
#endif

/**************************************************************************\
|******************************** - MACRO - *******************************|
\**************************************************************************/
#define POS_CONVERT(x)	(((int)x - (BC_PMx-BC_PMn)/2.0) * MAX_POS*2.0 / (BC_PMx-BC_PMn))    // BY Yann
#define ANG_CONVERT(x)	(((int)x - BC_A0 ) * MAX_ANGL*2.0 / (BC_AMx-BC_AMn))               // By Yann

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>

//#include "3712.h"

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
/* Matrice */
float Adc[4][4];
float Bdc[4][2];
float Cdc[4];
float x[4];
float x_save[4];
float y[2];
unsigned int didit;

/**************************************************************************\
|************************ - PROTYPE DES FONCTIONS - ***********************|
\**************************************************************************/
/* LANCEMENT */
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
/* DRIVER CAN */
void 	init_can 		(void);
void 	emission 		(u16 id,u8 *data,u8 lenght,u8 RTR_bit);
void 	reception 		(int focus, int * data);
/* DRIVER DAC */
void 	set_DA 			(int canal, unsigned int value_n);
/* DRIVER ADC */
void 	init_3718 		(void);
void 	trigger 		(void);
int 	adc_read_eoc 	(void);
int 	adc_read_value 	(void);
void 	ad_range_select	(int canal, int range);
/* MATRICE */
void  	init_matrix 	(void);
float 	calc_matrix 	(void);
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

	printk("prog_init :\n\tBanc : %d\n\tBC_PMn = %d\n\tBC_PMx = %d\n\tBC_AMn = %d\n\tBC_AMx = %d\n", BANC, BC_PMn, BC_PMx, BC_AMn, BC_AMx );

	init_matrix();		// initialisation des matrices
	init_3718();		// init de l'adc
#if MODE_PENDULE == 2
	init_can();			// Initialisation du bus CAN
	// Tache d'acquisition/action
	ierr_in = rt_task_init(&tache_in,task_in,0,STACK_SIZE, PRIORITE, 1, 0);
	rt_task_make_periodic(&tache_in, now, nano2count(PERIODE_CONTROL));
	// Tache de traitement déporter
	ierr_out = rt_task_init(&tache_out,task_out,0,STACK_SIZE, PRIORITE, 1, 0);
	rt_task_make_periodic(&tache_out, now, nano2count(PERIODE_CONTROL));
	//Interuption de lecture de donnée.
	rt_global_cli();									// Desactivation des IT
	rt_request_global_irq(5,routine_reception);			// Association de l interruption avec la routine
	rt_startup_irq(5);									// Activation de la ligne d interruption
	rt_global_sti();									// Re-activation des IT
#elif MODE_PENDULE == 1
	// Tache d'asservissement sans communication CAN
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
	stop_rt_timer();
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
 * attend la reception de la donnée, puis génère le voltage de la commande reçu
 */
void task_in(long arg)
{
#if DEBUG_AFF_ROUTINE >= 1
	printk("task_in : init\n");
#endif
	u8 data_send_in[4];
	unsigned int adc_value, angle_num_in=0, pos_num_in=0;
	glb_task_in_wait = 1;
	command_in = 0; 			// Initialisation en cas d'erreur
	while(1)
	{
		/* Acquisition */
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
		/* Envoi de donnée */
#if DEBUG_AFF_ROUTINE >= 2
			printk("task_in : send angle_num_in = %d & pos_num_in = %d\n",angle_num_in, pos_num_in);
#endif
		do
		{
#if DEBUG_AFF_ROUTINE >= 3
			printk("task_in : retry to send\n");
#endif
			data_send_in[0] = ((angle_num_in >> 8) & 0x0F) + CAN_COMM_ACQUISITION;
			data_send_in[1] = angle_num_in & 0xFF;
			data_send_in[2] = (pos_num_in >> 8) & 0x0F;
			data_send_in[3] = pos_num_in & 0xFF;
			emission(CAN_SEND_ID,data_send_in, 4, 0);
			/* Swich de routine en attendant la réponse.... */
			rt_task_wait_period();
			/* Attente de reception */
		}
		while(glb_task_in_wait);
		glb_task_in_wait = 1;
		/* Actionneur */
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_in : received command_in = %d\n",command_in);
#endif
		set_DA(0, command_in);	// on ecrit dans le canal 0, la "commande"
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
	while(1)
	{
#if DEBUG_AFF_ROUTINE >= 2
		printk("task_out : received angle_num_out = %d & pos_num_out = %d\n",angle_num_out, pos_num_out);
#endif
		/* Conversion */
	    y[1] = POS_CONVERT(pos_num_out);
		y[0] = ANG_CONVERT(angle_num_out);
		/* Traitement */
#if TEST
		command_out = TEST_COMMANDE_OUT;
#else
		command_out = (int)((calc_matrix() + 10.0) * 4095.0 / 20.0); // Convertion Volt/numerique
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
		command_one = (int)((calc_matrix() + 10.0) * 4095.0 / 20.0); 					// Convertion Volt/numerique
		command_one = command_one<0 ? 0 : command_one>4095 ? 4095 : command_one;
#if DEBUG_AFF_ROUTINE == 3
		/** Affichages des angles **/
		printk("task_one :\n\tangle_num =\t%d\n\tangle_rad =\t",angle_num_one,pos_num_one);
		affichage_float(y[0]);
		printk("\n");
#elif DEBUG_AFF_ROUTINE == 4
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
		set_DA(0, command_one);	// on ecrit dans le canal 0, la "commande"
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
|***************************** - DRIVER CAN - *****************************|
\**************************************************************************/
/*
 * init_can : Fonction d'initialisation du bus CAN
 */
void init_can(void)
{
	outb(0x01,CAN_CONTROL);
	outb(0x03,CAN_CONTROL);				// Activation des interruptions en cas de reception
	outb(0x00,CAN_ACCEPTANCE_CODE);
	outb(0x00,CAN_ACCEPTANCE_MASK);
	outb(0x03,CAN_BUS_TIMING_0);
	outb(0x1C,CAN_BUS_TIMING_1);
	outb(0xFA,CAN_OUTPUT_CTRL);
	inb(CAN_INTERRUPT);					// Lecture du registre d interruptions pour terminer leur activation
	outb(0x02,CAN_CONTROL);
#if DEBUG_AFF_CAN >= 1
	printk("init_can done\n");
#endif
}
/*
 * Fonction d emission sur le bus CAN
 */
void emission(u16 id,u8 *data,u8 lenght,u8 RTR_bit)
{
	u8 id_p1, id_p2;
	id_p1 = id >> 3;
	id_p2 = ((id & 0x007) << 5) + (RTR_bit*16) + (lenght&0x0F);	// Securite sur la longueur pour etre sur qu elle ne depasse pas 4 bits
#if DEBUG_AFF_CAN >= 1
	printk("emission : id 1ere partie :\t0x%x\n", id_p1);
	printk("emission : id,RTR,longueur :\t0x%x\n", id_p2);
	printk("emission : data :\t\t0x%x\n", data[0]);
#endif
	outb(id_p1,CAN_TX_IDENTIFIER);
	outb(id_p2,CAN_TX_RTB_BIT);
	
	if(inb(CAN_STATUS) && 0x04)		// Verifie que les buffers d emission sont libres
	{
		int i;
		// premiers buffers contiennent poids forts de la data a envoyer
		for(i=0 ; i<lenght ; i++)
		{
			/*outb((data >> ((lenght-1-i)*8)) & 0xFF , CAN_TX_BUFFER+i);*/
			outb(data[i] , CAN_TX_BUFFER+i);
		}
		while(!(inb(CAN_STATUS) && 0x08));	// Verifie que la transmission precedente s est bien terminee avant de lancer la nouvelle
		outb(0x01,CAN_COMMAND);	// Lance la transmission
	}
}

/*
* Cette fonction permet de lire le buffer de reception
* si reception il y a 
*/
void reception(int focus, int * data)
{	
	u8 rx_msg[8];
	unsigned int rx_msg_complet;
	int rx_size;
	u16 rx_id;
	int j;
#if DEBUG_AFF_CAN_REC >= 1
	printk("reception : message received...\n");
#endif
	if(inb(CAN_STATUS) && 0x01) //reception!
	{
		rx_id = (inb(CAN_RX_IDENTIFIER) << 3) + (inb(CAN_RX_RTB_BIT) >> 5);
		
		if((rx_id == focus) || (focus == 0))
		{
			rx_size = (inb(CAN_RX_RTB_BIT)) & 0x0F ;
#if DEBUG_AFF_CAN_REC >=2
			printk("reception :\n\tid :\t %d\n\tsize :\t%d\n",rx_id,rx_size);
#endif
			for(j=0; j < rx_size; j++)	// On recupere d abord les buffer ayant les data de poids fort
			{
				rx_msg[j] = inb(CAN_RX_BUFFER+j);
#if DEBUG_AFF_CAN_REC >=3
				printk("Buf %d :\t 0x%x\n",j,rx_msg[j]);
#endif
			}
			
			outb(0x04,CAN_COMMAND);	// Release du buffer de reception
			
			rx_msg_complet = rx_msg[0];
			for(j=0; j < rx_size; j++)
			{
				if(j%2 == 0){data[j/2] = rx_msg[j]<<8; 		}
				else 		{data[j/2] += (rx_msg[j]); 	}
				//rx_msg_complet = (rx_msg_complet << 8) + rx_msg[j];
			}
		}
	}
}

/**************************************************************************\
|***************************** - DRIVER DAC - *****************************|
\**************************************************************************/
/*
 * set_DA
 * Function output: send in the channel "canal" the voltage "value" in volt (+-10V).
 * 8 LSB d'abord, 8 MSB après. Buffer garde resultat et n'envoie que quand MSB est écrit
 * Valeur à rentrer de -10 a 10
 */
void set_DA(int canal, unsigned int value_n)
{
	int lsb, msb;
#if DEBUG_AFF_DAC >= 1 
	printk("set_DA\n");
#endif
	lsb = 0;
	msb = 0;
	lsb = value_n & 0x00FF;			//Recuperation du LSB
	msb = value_n >> 8;				//Recuperation du MSB
#if DEBUG_AFF_DAC >= 2
	printk("Valeur en bits : %d\n",value_n);
	printk("Valeur MSB : 0x%x\n",msb);
	printk("Valeur LSB : 0x%x\n",lsb);
#endif
	/* WRITE OUTPUT */
	outb(lsb,BASE_DAC_0+2*canal);
	outb(msb,BASE_DAC_1+2*canal);
}

/**************************************************************************\
|***************************** - DRIVER ADC - *****************************|
\**************************************************************************/
/*
 * Pour lire une valeur de l'ADC (une fois init):
 * trigger() -> adc_read_eoc() -> adc_read_value()
 */
void init_3718(void)
{    //Pour lire canal 0, mettre 0x00. Pour lire canaux 0 et 1 alternativement, mettre 0x10
	ad_range_select(0x00,8);	// Canal 0, +/-10V
	ad_range_select(0x11,8);	// Canal 1, +/-10V
    outb(0x10, BASE_ADC_2);		// selectionne le canal   
}

/* 
 * Trigger the A/D conversion by writing to the A/D low byte register (BASE+0) with any value.
 */
void trigger(void)
{
    outb((char)0xFF,BASE_ADC_0);
}

/*
 * ajout de cette fonction pour permettre une interruption quand fin de de conversion
 */
int adc_read_eoc(void)
{    
    int eoc;
    int temp;
    eoc = inb(BASE_ADC_8);					// EOC: end of convertion p41/37
    if((eoc & 0x80) == 0x80){temp = 1;}		// verif fin conversion
    else{ temp = 0;}
    return temp;
}

/*
 * lecture de la valeur une fois que la conversion est finie
 */
int adc_read_value(void)
{
    int lsb		= 0;
    int msb		= 0;    
    int res		= 0;
    int chan 	= 0;
    
    chan 	= inb(BASE_ADC_0);					// recuperation du LSB
    msb 	= inb(BASE_ADC_1);					// recuparation du MSB
    lsb 	= chan >> 4;						// recuperation LSB
    chan 	= chan & 0x0F;						// recuperation canal de la valeur lue
    res 	= (((msb << 4) + lsb)<< 4 )+ chan;	// Convertion de la valeur + concatenation du canal dans les 4 bits de poids faible

    return res;
}


/*
 * ad_range_select
 * Set the input range for each A/D channel. (p26/30))
 * base+1 D3 to D0 = b1000
 */
void ad_range_select(int canal, int range)
{
	outb((char)canal, BASE_ADC_2);	// selectionne le canal
	outb((char)range, BASE_ADC_1);	// met le range
}

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

/*
* Matrice de CELA selon SAGLAM *
*float A[4][4]={{0.6300,   -0.1206,   -0.0008,    0.0086},
                {-0.0953,    0.6935,    0.0107,    0.0012},
                {-0.2896,   -1.9184,    1.1306,    0.2351},
                {-3.9680,   -1.7733,   -0.1546,    0.7222}};

    float B[4][2]={ {0.3658,    0.1200},
                    {0.0993,    0.3070},
                    {1.0887,    2.0141},
                    {3.1377,    1.6599}};
* Matrice de SAGLMA *
float A[4][4]={{0.6601,   0.0680,   -0.0008,    0.0086},
    {0.0662,    0.7861,    0.0107,    0.0012},
    {1.3274,   -0.8919,    1.1306,    0.2351},
    {-3.1767,   0.4320,   -0.1546,    0.7222}};

    float B[4][2]={{0.3358,    -0.0686},
        {-0.0622,    0.2144},
        {-0.5283,    0.9876},
        {2.3464,    -0.5454}};

float C[4]={-80.3092,   -9.6237,  -14.1215,  -23.6260};

*/
#if MODE_PENDULE == 1
	//Matrice un seul pendule
	/*Adc[0][0]= 0.6196;    Adc[0][1]= 0.0968;    Adc[0][2]=-0.0008;    Adc[0][3]= 0.0086;
	Adc[1][0]= 0.0971;    Adc[1][1]= 0.7038;    Adc[1][2]= 0.0107;    Adc[1][3]= 0.0012;
	Adc[2][0]= 1.8124;    Adc[2][1]=-1.7997;    Adc[2][2]= 1.1306;    Adc[2][3]= 0.2351;
	Adc[3][0]=-3.8837;    Adc[3][1]= 0.8724;    Adc[3][2]=-0.1546;    Adc[3][3]= 0.7222;
	
	Bdc[0][0]= 0.3762;    Bdc[0][1]=-0.0973;
	Bdc[1][0]=-0.0931;    Bdc[1][1]= 0.2966;
	Bdc[2][0]=-1.0133;    Bdc[2][1]= 1.8954;
	Bdc[3][0]= 3.0534;    Bdc[3][1]=-0.9858;*/
	
	Adc[0][0]= 0.6200;    Adc[0][1]= 0.0816;    Adc[0][2]=-0.0013;    Adc[0][3]= 0.0080;
	Adc[1][0]= 0.0939;    Adc[1][1]= 0.5994;    Adc[1][2]= 0.0111;    Adc[1][3]= 0.0017;
	Adc[2][0]= 2.3148;    Adc[2][1]=-3.1690;    Adc[2][2]= 1.1223;    Adc[2][3]= 0.3316;
	Adc[3][0]=-4.4627;    Adc[3][1]= 0.6851;    Adc[3][2]=-0.2633;    Adc[3][3]= 0.6079;
	
	Bdc[0][0]= 0.3739;    Bdc[0][1]=-0.0828;
	Bdc[1][0]=-0.0882;    Bdc[1][1]= 0.4016;
	Bdc[2][0]=-1.1846;    Bdc[2][1]= 3.3700;
	Bdc[3][0]= 3.2403;    Bdc[3][1]=-0.9231;

	Cdc[0]=-113.5831;    Cdc[1]=-20.1981;    Cdc[2]=-23.3416;    Cdc[3]=-33.3277;

#elif MODE_PENDULE == 2
	//Matrice deux pendules (Allah vérifier) (NE MARCHE PAS POUR PENDULE SEUL)
	Adc[0][0]= 0.6300;    Adc[0][1]=-0.1206;    Adc[0][2]=-0.0008;    Adc[0][3]= 0.0086;
	Adc[1][0]=-0.0953;    Adc[1][1]= 0.6935;    Adc[1][2]= 0.0107;    Adc[1][3]= 0.0012;
	Adc[2][0]=-0.2896;    Adc[2][1]=-1.9184;    Adc[2][2]= 1.1306;    Adc[2][3]= 0.2351;
	Adc[3][0]=-3.9680;    Adc[3][1]=-1.7733;    Adc[3][2]=-0.1546;    Adc[3][3]= 0.7222;

	Bdc[0][0]=0.3658;    Bdc[0][1]=0.1200;
	Bdc[1][0]=0.0993;    Bdc[1][1]=0.3070;
	Bdc[2][0]=1.0887;    Bdc[2][1]=2.0141;
	Bdc[3][0]=3.1377;    Bdc[3][1]=1.6599;

	Cdc[0]=-80.3092;    Cdc[1]=-9.6237;    Cdc[2]=-14.1215;    Cdc[3]=-23.6260;
#endif

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
float calc_matrix(void)
{
	/* Declaration de variable */
	float commande; // commande : varaible de sortie
#if DEBUG_AFF_MAT >= 1
	printk("calc_matrix:\n"); // Affichage de debugage niveau 1.
#endif
	/* CALCULE DE LA COMMANDE */
	/*
	x[0]=y[0];
	x[1]=y[1];
	x[2]=vit_ang;
	x[3]=vit_pos;
	*/
	/** CALCULE DE LA MATRICE D'ETAT X = Adc * X + Bdc * Y **/
	x_save[0]= Adc[0][0]*x[0] + Adc[0][1]*x[1] + Adc[0][2]*x[2] + Adc[0][3]*x[3] + Bdc[0][0]*y[0] + Bdc[0][1]*y[1];
	x_save[1]= Adc[1][0]*x[0] + Adc[1][1]*x[1] + Adc[1][2]*x[2] + Adc[1][3]*x[3] + Bdc[1][0]*y[0] + Bdc[1][1]*y[1];
	x_save[2]= Adc[2][0]*x[0] + Adc[2][1]*x[1] + Adc[2][2]*x[2] + Adc[2][3]*x[3] + Bdc[2][0]*y[0] + Bdc[2][1]*y[1];
	x_save[3]= Adc[3][0]*x[0] + Adc[3][1]*x[1] + Adc[3][2]*x[2] + Adc[3][3]*x[3] + Bdc[3][0]*y[0] + Bdc[3][1]*y[1];
	/** CALCULE DE LA COMMANDE commande = u = -Cdc * X **/
	commande =  + Cdc[0]*(x_save[0])
				+ Cdc[1]*(x_save[1])
				+ Cdc[2]*(x_save[2])
				+ Cdc[3]*(x_save[3]);
#if DEBUG_AFF_MAT >= 2
	// Affichage de debugage niveau 2
	printk("calc_matrix :\n\tx =\t\t");affichage_float(x[0]);printk(" ; ");affichage_float(x[1]);printk(" ; ");affichage_float(x[2]);printk(" ; ");affichage_float(x[3]);
	printk("\n\tx+1 =\t\t");affichage_float(x_save[0]);printk(" ; ");affichage_float(x_save[1]);printk(" ; ");affichage_float(x_save[2]);printk(" ; ");affichage_float(x_save[3]);
	printk("\n\ty =\t\t");affichage_float(y[0]);printk(" ; ");affichage_float(y[1]);
	printk("\n\tcommande = \t"); affichage_float(commande);
	printk("\n");
#endif
	/** MEMORISATION DE LA MATRICE D'ETATS ACTUEL **/
	x[0]=(x_save[0]);
	x[1]=(x_save[1]);
	x[2]=(x_save[2]);
	x[3]=(x_save[3]);
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
