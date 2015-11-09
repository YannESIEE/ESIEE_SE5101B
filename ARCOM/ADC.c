
/**************************************************************************\
|********************************* - ADC - ********************************|
\**************************************************************************/
/* Adresse des registres */
#define BASE_ADC_0 0x0320
#define BASE_ADC_1 (BASE_ADC_0+1)
#define BASE_ADC_2 (BASE_ADC_0+2)
#define BASE_ADC_3 (BASE_ADC_0+3)
#define BASE_ADC_8 (BASE_ADC_0+8)

/* DRIVER ADC */
void 	init_3718 		(void);
void 	trigger 		(void);
int 	adc_read_eoc 	(void);
int 	adc_read_value 	(void);
void 	ad_range_select	(int canal, int range);

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
