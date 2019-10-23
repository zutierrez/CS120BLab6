/*	Author: zguti001
 *      Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: Reflex Game
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

enum States { FIRST, SECOND, THIRD, WAIT_P, WAIT_R } state;

unsigned char outputB = 0x00;
unsigned char dir_flag = 0x00;
unsigned char inputA = 0x00;

void SM(){
	
	switch(state){  //transitions
		
		case FIRST:
			if(inputA == 0x01) {
				state = WAIT_P;
			}
			else { state = SECOND; }
			break;
			
		case SECOND:
			if(inputA == 0x01) {
                                state = WAIT_P;
                        }
                        else {
			if( dir_flag == 0){
				state = THIRD;
			}
			if( dir_flag == 1){
				state = FIRST;
			}
			}
			break;

		case THIRD:
			if(inputA == 0x01) {
                                state = WAIT_P;
                        }
                        else { state = SECOND; }
			break;
		
		case WAIT_P:
			if(inputA == 0x01) {
                                state = WAIT_P;
                        }
                        else if(inputA == 0x00) {
				state = WAIT_R;
			}	
			break;
		
		case WAIT_R:
			if(inputA == 0x00) {
                                state = WAIT_R;
                        }
                        if(inputA == 0x01 && outputB == 0x01) {
                                state = FIRST;
                        }
			if(inputA == 0x01 && outputB == 0x02) {
                                state = SECOND;
                        }
			if(inputA == 0x01 && outputB == 0x04) {
                                state = THIRD;
                        }
			break;

		default:
			state = FIRST;
			break;			
	}

	switch(state){   //actions

		case FIRST:
			dir_flag = 0;
			outputB = 0x01;
			break;	
		
		case SECOND:
			outputB = 0x02;
			break;

		case THIRD:
			dir_flag = !dir_flag;
			outputB = 0x04;
			break;	

		case WAIT_P:
			break;

		case  WAIT_R:
			break; 

	}
}


int main(void) {
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's pins as outputs
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's pins as inputs
	TimerSet(300);
	TimerOn();

    	enum States state = FIRST;

	while (1) {
		inputA = PINA;
		inputA = ~inputA;
		SM();
		PORTB = outputB;
		while(!TimerFlag){};
		TimerFlag = 0;

	}
    return 1;
}
