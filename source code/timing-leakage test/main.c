#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include <driverlib.h>
#include <uart_fifo.h>
#include <sw_crypto/TI_aes_128.h>
#include <timer_a.h>
#include <timer_a.c>
#include "testedcode.h"

#define interrupt(x) void __attribute__((interrupt (x)))

#define TRIGGER_ACTIVE() GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3)
#define TRIGGER_RELEASE() GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3)

//Configurations
#if defined(test1TX) 
	#define inputlen 31
	#define TX
#elif defined(test1RX)
	#define inputlen 31
	#define RX
	#define helperlen 3
	#define t 1
#elif defined(test2TX) 
	#define inputlen 31
	#define TX
#elif defined(test2RX)
	#define inputlen 31
	#define RX
	#define helperlen 5
	#define t 2
#elif defined(test3TX)
	#define inputlen 63
	#define TX
#elif defined(test3RX)
	#define inputlen 63
	#define RX
	#define helperlen 15
	#define t 7
#elif defined(test4TX) 
	#define inputlen 50
	#define TX
#elif defined(test4RX)
	#define inputlen 50
	#define RX
	#define helperlen 2
#elif defined(test5TX)
	#define inputlen 20
	#define TX
#elif defined(test5RX)
	#define inputlen 20
	#define RX
	#define helperlen 2
#elif defined(test6TX)
	#define inputlen 10
	#define TX
#elif defined(test6RX)
	#define inputlen 10
	#define RX
	#define helperlen 2
#endif 


/**
 9600 Baud 8/n/1
 Send: 'e' (0x65) + input
 Receive: CPU cycles cost per execution
*/

/*
 * GPIO Initialization
 */
static void Init_GPIO()
{
    // Set all GPIO pins to output low for low power
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7|GPIO_PIN8|GPIO_PIN9|GPIO_PIN10|GPIO_PIN11|GPIO_PIN12|GPIO_PIN13|GPIO_PIN14|GPIO_PIN15);
	
    // Set PJ.4 and PJ.5 as Primary Module Function Input, LFXT.
    GPIO_setAsPeripheralModuleFunctionInputPin
	(
	   GPIO_PORT_PJ,
	   GPIO_PIN4 + GPIO_PIN5,
	   GPIO_PRIMARY_MODULE_FUNCTION
    );
		   
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);
	GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PMM_unlockLPM5();
}

/*
 * Clock System Initialization
 */
static void Init_Clock()
{
    // Set DCO frequency to 8 MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);
	
    //Set external clock frequency to 32.768 KHz
    CS_setExternalClockSource(32768, 0);
	
    //Set ACLK=LFXT
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
	
    // Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
	
    // Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
	
    //Start XT1 with no time out
    CS_turnOnLFXT(CS_LFXT_DRIVE_0);
}


/* Timer code for measuring cycles

*/
static volatile uint64_t timer_ctr = 0;

 
volatile interrupt(TIMER0_A1_VECTOR) ctrinc(void)
{
	timer_ctr += (uint64_t)0x10000;

	TA0IV = 0;
}

 
static void Init_TimerA()
{
	Timer_A_initContinuousModeParam param = {0};
	param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
	param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_8;
	param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
	param. timerClear = TIMER_A_DO_CLEAR;
	param.startTimer = true;

	Timer_A_initContinuousMode(TIMER_A0_BASE, &param);
}

 
uint64_t cpucycles()
{
	return ((uint64_t)timer_ctr | (uint64_t)TA0R) << 3;
}



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
	Init_GPIO();
	
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
	
	TRIGGER_RELEASE();
	
	// Toggle LED1 and LED2 to indicate start
	int i;
	for (i = 0; i < 10; i++)
	{
		GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
		GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
		__delay_cycles(200000);
	}
	
	// 9600 Baud
	Init_Clock();
	uart_init();
	
	Init_TimerA();
	
	// Enable global interrupts
    __enable_interrupt();
	
	// Flush
	while(rx_flag)
	{
		uart_getc();
	}
	
	//always waiting
	while (1)
    {	
		while(rx_flag != 0)
		{
			
			uint8_t x = uart_getc();
			
			if(x == 'e')
			{
                //receive the random data from serial port 					
				#if defined(test1RX) || defined(test2RX) || defined(test3RX)
					int8_t input[inputlen];
					int8_t PScopy[inputlen];
					for (i = 0; i < inputlen; i++){
						input[i] = uart_getc();
						PScopy[i] = input[i];
					}
				#else
					uint8_t input[inputlen];
					for (i = 0; i < inputlen; i++){
						input[i] = uart_getc();
					}
				#endif
			
            	//calculate the CPU cycles on each program execution    
				uint64_t start;
				uint64_t duration;		
				
				//If testing TX
				#if defined TX
					start = cpucycles();

					testedcode(input);

					duration = cpucycles();
					duration -= start;
				
				//If testing RX
				#elif defined RX
				
					//prepare the helper data and error location
					#if defined(test1RX) || defined(test2RX) || defined(test3RX)
						int8_t helper[helperlen];
						getsyndrome(helper, PScopy);
						uint8_t errorloc[t];
						for (i = 0; i < t; i++){
							errorloc[i] = rand() % inputlen;
						}
					#elif defined(test4RX) || defined(test5RX) || defined(test6RX)
						int16_t helper[helperlen];
						calculHelper(helper, input);
						uint8_t errorloc = rand() % inputlen;
					#endif

					start = cpucycles();

					testedcode(helper, input, errorloc);

					duration = cpucycles();
					duration -= start; 				
				 	
				#endif
				
                //send the cycles to the serial port
				uint8_t timediff[8];
				for(i = 0; i < 8; i++){
					timediff[i] = 0xFF & (duration >> 8*i);		
				}
				
				__delay_cycles(200000);
				for(i = 7; i >= 0; i--){
					uart_putc(timediff[i]);		
					__delay_cycles(200000);
				}
				
			}	
		}
	}
} 
