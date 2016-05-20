// BomberGame.c
// Runs on LM4F120/TM4C123
// 

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "drawings.h"
#include "porte.h"
#include "ADC.h"
#include "sound.h"


// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
#ifdef __TI_COMPILER_VERSION__
	//Code Composer Studio Code
	void Delay(uint32_t ulCount){
	__asm (	"    subs    r0, #1\n"
			"    bne     Delay\n"
			"    bx      lr\n");
}

#else
	//Keil uVision Code
	__asm void
	Delay(uint32_t ulCount)
	{
    subs    r0, #1
    bne     Delay
    bx      lr
	}
#endif

	
  
	
	
volatile  long ADCvalue = 0 , oldValue = 0 ;	
char lock = 0 ;

	

	
// Subroutine Defines interrupt on systemTick
// Inputs:  none
// Outputs: sound on the speaker
// Notes:   friendly means it does not affect other bits in the port
void SysTick_Handler(void){

	int c = get_counter();
		
	
	if(c)
		GPIOB->DATA ^=  ( 1 << 2 ) ;
		else 
		GPIOB->DATA = 0x00;
	
		set_counter(c ^ 0x01);
		
		
		switch(get_func())
		{	
			case 'w':
				start_sound();
				break;
			case 'd':
				end_sound();
				break;
			case 'r':
				bomber_sound();
				break;	
			case 'b':
				bomb_sound();
				break;	
			case 'y':
				enemy_sound();
				break;
		}		
	}
		

	

	// Subroutine Defines interrupt on portE
// Inputs:  none
// Outputs: LEDs lit up
// Notes:   friendly means it does not affect other bits in the port
void GPIOPortE_Handler(void)
{
				interrupt_ackn();
			
				
				if( ( GPIOE->DATA & 0x01) == 0)
				shoot();
				
				if ( ( GPIOE->DATA & 0x10) == 0)
				{ 
				lock ^= 1 ;
				if (lock)
				pauser();
				}
				
				Delay1ms(30);	
}











int main(void){
  
	uint32_t score = 0;
	
	PortE_Init();
	init_interrupt();
	Sound_Init();       
	PLL_Init();                           // set system clock to 50 MHz
  ADC0_InitSWTriggerSeq3_Ch1();         // ADC initialization PE2/AIN1	
	Nokia5110_Init();
	
	start_sound();				// output start sound on speaker
	welcomer();						// output welcome screen on start
	ADCvalue = ADC0_InSeq3() ;		// read the value of the initial slier pot 
	start_pos(ADCvalue);				// locate bombrer at the initial postition
  oldValue = ADCvalue;
	stop_sound();				// stop the start sound


	while(1){
   
		
				if(!lock)		// game is on while we don't pause it by pause switch
					{
		
							ADCvalue = ADC0_InSeq3() ;   // read the value of the slier pot
						  score += check_death();		
							update_diaplay();			// display the new objects positions on screen
							update_pos(oldValue , ADCvalue);	// upddate objects positions with time	
							oldValue = ADCvalue;
							score++;		
					}	
			
	
					if (getLive() == 0)			// stop game if live points = 0
						break;					
				}
			
			end_sound();			// the game over sound on speaker
			gameOver(score / 10);			// display game over on screen with your score
				while(1) ;				// block the game until micro is reseted
	
}


