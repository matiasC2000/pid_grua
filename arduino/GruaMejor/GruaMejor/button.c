/*
 * button.c
 *
 * Created: 3/8/2024 9:05:50 AM
 *  Author: Calvo
 */ 
#include <avr/io.h>
#include "global.h"

uint8_t static contAON=0,contAOFF=0,contB=0;
uint8_t FLAG_button_A = 0;
uint8_t FLAG_button_B = 0;
uint8_t FLAG_button_A_change=0;

void button_init(){
	DDRD &= ~(1<<PORTD7); //pongo D7 como entrada
	PORTD |= (1<<PORTD7); //pongo D7 como pull up
	
	DDRB &= ~(1<<PORTB0); //pongo B0 como entrada
	PORTB |= (1<<PORTB0); //pongo B0 como pull up
}


void button_Actulizar(){
	if(PIND & (1<<PORTD7)){
		//apretado
		contAOFF=0;
		if(++contAON>15){
			FLAG_button_A=1;
			contAON=100;
		}
	}else{
		//lo solto
		contAON=0;
		if(++contAOFF>15){
			FLAG_button_A=0;
			contAOFF=100;
		}
		if(contAOFF==13){
			FLAG_button_A_change=1;
		}
	}
	if(! (PINB & (1<<PORTB0))){
		if(++contB>10){
			FLAG_button_B=1;
		}
	}else{
		contB=0;
	}
}