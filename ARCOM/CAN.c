
/**************************************************************************\
|********************************* - CAN - ********************************|
\**************************************************************************/
/* Affichage et débugage */
#define DEBUG_AFF_CAN 		0 // niveau trois est un peu HARD pour arcom
#define DEBUG_AFF_CAN_REC	0

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

#include <asm/io.h>

/* DRIVER CAN */
void 	init_can 		(void);
void 	emission 		(u16 id,u8 *data,u8 lenght,u8 RTR_bit);
void 	reception 		(int focus, int * data);

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
