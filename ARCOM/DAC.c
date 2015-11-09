
/**************************************************************************\
|********************************* - DAC - ********************************|
\**************************************************************************/
/* AFFICHAGE : Attention, il est conseillé de ne pas activé tous les affichage à la fois */
#define DEBUG_AFF_DAC 		0
/* Adresse de registre */
#define BASE_DAC_0 0x0300
#define BASE_DAC_1 (BASE_DAC_0+1)
#define BASE_DAC_2 (BASE_DAC_0+2)
#define BASE_DAC_3 (BASE_DAC_0+3)
#define BASE_DAC_8 (BASE_DAC_0+8)

/* DRIVER DAC */
void 	set_DA 			(int canal, unsigned int value_n);

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
