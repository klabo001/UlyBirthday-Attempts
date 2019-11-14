/*
 * UlyBirthday.c
 *
 * Created: 11/13/2019 11:09:04 PM
 *  Author: klabo
 */ 

#include <avr/io.h>

/*
 * ULYBDAY.c
 *
 * Created: 11/6/2019 7:47:06 PM
 *  Author: klabo
 */ 
/*	Author: kmaka003
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>
volatile unsigned char TimerFlag = 0; //TimerISR() sets this to a 1. C programmer should clear to 0.
// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; //Start count from here, down to 0. Default to 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1 ms ticks

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADENl: setting this bit enables analog to digital conversion
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto triggering.
	// Since we are in Free Running Mode, a new conversion
	// will trigger whenever the previous conversion completes
	
	//
}

void TimerOn()
{
	//AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; //bit3 = 0: CTC mode (clear timer on compare)
	//bit2bit1bit0 = 011: pre-scaler /64
	// 00001011: 0x0B
	// so, 8MHz clock or 8,000,000 /64 =125,000 ticks/s
	// Thus, TCNT1 register will count as 125,000 ticks/s
	//AVR output compare register OCR1A.
	OCR1A = 125;   // Timer interrupt will be generated when TCNT1 == OCR1A
	// We want a 1 ms tick. 0.001s *125,000 ticks/s = 125
	// so when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt
	
	// Initialize avr counter
	TCNT1=0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds
	
	//Enable global interrupts
	SREG |=0x80; // 0x80: 1000000
	
}

void TimerOff()
{
	TCCR1B = 0x00; // bit3bitbit0 -000: Timer off
}

void TimerISR()
{
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
	//CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) //results in a more efficient compare
	{
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


//0.954 hz is lowest frequency possible with this function,
//based on settings in PWN_on()
//Passing in 0 as the frequency will stop the speaker from generating sound

void set_PWM(double frequency){
	static double current_frequency; // keeps track of the currently set frequency
	//Will only update the registers when the frequency changes, otherwise allows
	//music to play uninterrupted
	
	if(frequency != current_frequency)
	{
		if(!frequency) {TCCR1B &= 0x08;}//stops timer/counter
		else {TCCR1B |= 0x03;} //resumes/continues timer/counter
			
		//prevents OCR3A from overflowing using prescaler 64
		//0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		
		//prevents OCR3A from underflowing using prescaler 64
		//31250 is largest frequency that will not result in underflow
		if (frequency > 31250) {OCR3A = 0x0000;}
			
		else { OCR3A = (short) (8000000/(128*frequency))-1; }
			
			TCNT3 = 0; //resets counter
			current_frequency = frequency; // updates the current frequency
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
		//COM3A0: Toggle PB3 on compare match between counter and OCR3A 
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
		// wGM32: When counter (TCNT3) matches OCR3A, reset counter
		//CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_oFF (){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

double NOTES[] = {440.00, 493.88, 261.63, 293.66, 329.63, 349.23, 392.00, 0};
//int song[] = {7, 7, 1, 7, 3, 2, 7, 7, 1, 7, 4, 3, 7, 7, 7, 5, 3, 2, 1, 6, 6, 5, 3, 4, 3, 8};
int song[] = {7,1,2,3,4};	
enum Song{init, next, finish } state;
#include <avr/io.h>
void SongSM(unsigned char currnote, int song[], double NOTES[]){
		
	set_PWM(NOTES[song[currnote]-1]);
};
int main()
{
	//25 notes
	
	DDRA= 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
// 	const unsigned long timerPeriod = 1;
// 	unsigned currnote = 0;
// 	state = init;
// 
// 	TimerSet(1);
// 	TimerOn();
// 	PWM_on();
// 	

	unsigned char A0 = ~PINA & 0x01;
	
    while(1)
    {
				
		if(A0)
		{
			PORTB = 0x0F;  
		}
	}
}
