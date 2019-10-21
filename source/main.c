/*	Author: zguti001
 *      Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; 

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;

	OCR1A = 125;
	
	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;

}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet( unsigned long M) {
	_avr_timer_M = M; 
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States { INIT, FIRST, SECOND, THIRD, WAIT } state;

unsigned char outputB = 0x00;
unsigned char last_LED = 0x00;

void SM(){
	
	switch(state){  //transitions
		
		case INIT:
			outputB = 0x01;
			state = FIRST;

		case FIRST:
			last_LED = 1;
			state = SECOND;
			break;			
		case SECOND:
			if( last_LED = 1){
				state = THIRD;
			}
			if( last_LED = 0){
				state = FIRST;
			}
			break;
		case THIRD:
			last_LED = 0;
			state = SECOND;
			break;
		case WAIT:
			break;

		default:
			state = FIRST;
			break;			
	}

	switch(state){   //actions
		
		case INIT: 
			outputB = 0x01;
			break;

		case FIRST:
			last_LED = 1;
			outputB = 0x01;
			break;			
		case SECOND:
			outputB = 0x02;
			break;
		case THIRD:
			last_LED = 0;
			outputB = 0x04;
			break;
		case WAIT: 
			break;

	}
}


int main(void) {
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 7 most significant pins as
	TimerSet(1000);
	TimerOn();
	//unsigned char tmpB = 0x00;

    	enum States state = FIRST;

	while (1) {
		//inputA = PINA;
		SM();
		PORTB = outputB;
		while(!TimerFlag);
		TimerFlag = 0;

	}
    return 1;
}
