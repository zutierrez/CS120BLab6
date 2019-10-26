/*	Author: Zion Gutierrez, zguti001
 *      Partner(s) Name: none
 *	Lab Section: 023
 *	Assignment: Lab #4 Exercise #2
 *	Exercise Description: INCREMENTER AND DECREMENTER 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>

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


enum States { WAIT , PRESS_INCREMENT, RELEASE_INCREMENT, PRESS_INCREMENT_HOLD, PRESS_DECREMENT, RELEASE_DECREMENT, PRESS_DECREMENT_HOLD, PRESS_RESET, RELEASE_RESET } state;

unsigned char inputA = 0x00;
unsigned char cnt = 0x00;
unsigned char holdcnt = 0x00;

void SM()
{
	switch(state) {      //TRANSITIONS
	
		case WAIT:
	       		if ( inputA == 0x01 ){
				if( cnt < 9){
					state = PRESS_INCREMENT;
				}
	       		}
			else if( inputA == 0x02 ){
				if( cnt > 0 ){
					state = PRESS_DECREMENT;
				}
			}
			else if( inputA == 0x03 ){
				cnt = 0x00;
				state = PRESS_RESET;
			}
			else { state = WAIT; }
	       		break;

	    	case PRESS_INCREMENT:
			if( inputA == 0x01 && holdcnt < 30){
				++holdcnt;
				state = PRESS_INCREMENT;
			}
			if( inputA = 0x01 && holdcnt > 30){
				if ( cnt < 9){
					state = PRESS_INCREMENT_HOLD;
					holdcnt = 0;
				}
			} 
			else if( inputA == 0x03 ){
				cnt = 0x00;
				state = PRESS_RESET;
			}
			else if( inputA == 0x00) {
				 state = RELEASE_INCREMENT; }
	       		break;
		
		case PRESS_INCREMENT_HOLD:
			state = PRESS_INCREMENT;
			break;
	    	
		case RELEASE_INCREMENT: 
			++cnt;
			LCD_ClearScreen();
			LCD_WriteData(cnt + '0');
			state = WAIT;
			break;

	   	case PRESS_DECREMENT:
			if( inputA == 0x02 && holdcnt < 30){
				++holdcnt;
				state = PRESS_DECREMENT;
			}
			if( inputA = 0x02 && holdcnt > 30){
				if ( cnt > 0){
					state = PRESS_DECREMENT_HOLD;
					holdcnt = 0;
				} 
			}
			else if( inputA == 0x03 ){
				cnt = 0x00;
				state = PRESS_RESET;
			}
			else if( inputA == 0x00) {
				 state = RELEASE_DECREMENT; }
			break;

		case PRESS_DECREMENT_HOLD: 	
			state = PRESS_DECREMENT;
			break; 

		case RELEASE_DECREMENT:
			--cnt;
			LCD_ClearScreen();
			LCD_WriteData(cnt + '0');
			state = WAIT;
			break;

		case PRESS_RESET:
			if( inputA == 0x03 ){
				cnt = 0x00;
				LCD_ClearScreen();
				LCD_WriteData(cnt + '0');
				state = PRESS_RESET; }
			else if (inputA == 0x00){
				state = RELEASE_RESET; }
			break;

		case RELEASE_RESET:
			cnt = 0x00;
			state = WAIT;
			break;

		default:
	        	state = WAIT;
	       		break;	
		}	

	switch(state) {      //ACTIONS
	    
	    	case WAIT:		
			break;
	    	case PRESS_INCREMENT:
			break;
		case RELEASE_INCREMENT:
			break;
		case PRESS_INCREMENT_HOLD:
			++cnt;
			LCD_ClearScreen();
			LCD_WriteData(cnt + '0');
			break;
		case PRESS_DECREMENT:
			break;
		case RELEASE_DECREMENT:
			break;
		case PRESS_DECREMENT_HOLD:
			--cnt;
			LCD_ClearScreen();
			LCD_WriteData(cnt + '0');
			break;
		case PRESS_RESET:
			break;
		case RELEASE_RESET:
			break;
		}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // 
     	DDRC = 0xFF; PORTC = 0x00; // 
	DDRD = 0xFF; PORTD = 0x00;
	
	TimerSet(500);
	TimerOn();

	enum States state = WAIT;
	LCD_init();
	LCD_WriteData('0');

	while(1){ 
	//INPUT:
	inputA = PINA;
	inputA = ~inputA;
	
	SM();

	}  

	return 1;
}





