/*
 * AVR_Bootloader.c
 *
 * Created: 5/18/2021 8:49:27 PM
 * Author : Ahmed_Ayman
 */ 
#define F_CPU 16000000


#include <avr/io.h>
#include "Lcd-Driver.h"
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>


int main(void)
{
	uint8_t App [ 512 ]; /* start with app size 4 pages */
	uint8_t sreg;
	uint32_t page_Num = 0; /* here we have 4 pages form 0 -3 */
	uint16_t Avr_Word =0;
	unsigned char *buf = App;
					if (pgm_read_byte((void *)0) != 0xff) /* check if flash not progarmmed yet */
					{
						asm ( "jmp 0x0000" );
					}
	
	for (int iterator = 0; iterator < 512 ; iterator ++ ) /* read 512 bytes from EEPROM */
	{

		App[ iterator ] = eeprom_read_byte ((const uint8_t *) iterator);
		if((App[iterator] == 0xff)) /* here check if we reach to the end of application  */
		break;
	}
	LCD_Initializaion();
    /* Replace with your application code */
			LCD_Send_A_String_With_CurrLoc("Now Booting In ");
			LCD_Send_A_String_WithLoc(2,1,"    progress   ");
			_delay_ms(3000);
			LCD_Send_A_Command(0x01);

    while (1) 
    {
			/* Disable interrupts before start nay operation on the flash as explaned in the data sheet */ 
			sreg = SREG;
			cli();
		for ( page_Num =0;page_Num <4 ;page_Num++)	 /* iterate on the for pages */
		{

			eeprom_busy_wait ();
			boot_page_erase_safe (page_Num*SPM_PAGESIZE);    
			boot_spm_busy_wait ();      /* Wait until the memory is erased */

			for (int iterator=0; iterator<SPM_PAGESIZE; iterator+=2) /* fill the temp buffer */
			{
				Avr_Word = *buf++;
				Avr_Word += (*buf++) << 8;
				
				boot_page_fill_safe (iterator, Avr_Word); /* write to the temp flash buffer */
			}
			boot_page_write_safe (page_Num*SPM_PAGESIZE);     /* Store temp flash buffer in flash page */
			boot_spm_busy_wait();							 /* Wait until write operation is ended */
			boot_rww_enable ();


		}
				
			SREG = sreg;
			asm ( "jmp 0x0000" );
	}
	
    
}

