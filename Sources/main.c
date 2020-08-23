#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define VNTimer 11 /* Interrupt vector for Timer */ 

typedef	unsigned char	uint8;
typedef	unsigned short	uint16;
typedef	unsigned long	uint32;

typedef	char			int8;
typedef	short			int16;
typedef	long			int32;

// To Clear or Set single bits in a byte variable.
#define	b_SetBit(bit_ID, varID)		(varID |= (uint8)(1<<bit_ID))
#define	b_ClearBit(bit_ID, varID)	(varID &= ~(uint8)(1<<bit_ID))
#define	b_XorBit(bit_ID, varID)		(varID ^= (uint8)(1<<bit_ID))

// To Clear or Set single bits in a word(16-bit) variable.
#define	w_SetBit(bit_ID, varID)		(varID |= (uint16)(1<<bit_ID))
#define	w_ClearBit(bit_ID, varID)	(varID &= ~(uint16)(1<<bit_ID))
#define	w_XorBit(bit_ID, varID)		(varID ^= (uint16)(1<<bit_ID))

// To check single bit status in a given variable in ZERO PAGE
#define	b_CheckBit(varID, bit_ID)	((varID & (uint8)(1<<bit_ID))?1:0)
//#define	b_CheckBit(varID, bit_ID)	(varID & (muint8)(1<<bit_ID))
#define	w_CheckBit(varID, bit_ID) ((varID & (uint16)(1<<bit_ID))?1:0)

// To check single bit status in a given variable in ZERO PAGE
#define		DummyRead(varID)			{__asm lda varID;}


uint8 led_state = 1;


void PeriphInit(void);

void main(void)
{
	EnableInterrupts; /* enable interrupts */
	PeriphInit(); /* Microcontroller initialization */

	b_SetBit(0,PTFD);
	
	while(1)
	{
	}
}

interrupt VNTimer void TPM1_overflow()
{ 
	byte varTOF; 
	varTOF = TPM1SC_TOF; // clear TOF; first read and then write 0 to the flag
	TPM1SC_TOF = 0;
	if(led_state)
	{
		b_ClearBit(0,PTFD);
		led_state = 0;
	}
	else
	{
		b_SetBit(0,PTFD);
		led_state = 1;
	}
}

void PeriphInit()
{
	SOPT = 0x00; /* Disable COP - watchdog*/
	
	ICGC2 = 0X00; // Set up ICG control register 2
	ICGC1 = 0X78; // Set up ICG for FEE, 4MHz external crystal
	    // busclk = 8MHz

	// configure TPM module 1
	TPM1SC = 0x4F; // format: TOF(0) TOIE(1) CPWMS(0) CLKSB(0) CLKSA(1) PS2(1) PS1(1) PS0(1)	
	
	//8mhz = 8,000,000hz
	//prescaler 1:128 so our timer runs at
	//62,500 hz
	//and counts up to 65535.
	//to overflow once every second we need to count only up to 62,500, which is 0xF424
	TPM1MODH = 0xF4; // set the counter modulo registers	
	TPM1MODL = 0x24; 
	
	PTDPE = 0xFF; /* enable port D pullups for push button switch interrupt */
	
    PTFDD = 0xFF; // set PORTF direction as output
    PTFD = 0x00; // Turn OFF LEDs
	
	while (b_CheckBit(ICGS1,3) == 0){}
}
