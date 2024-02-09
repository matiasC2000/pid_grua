/*
 * Motor.c
 *
 * Created: 7/17/2023 4:27:08 PM
 * Author : Calvo
 */ 


#include "motor.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define IRIZQ PORTB &= ~(1<<PORTB1)

#define IRDER PORTB |= (1<<PORTB1)


int main(void)
{
	IRDER;
	
	uint16_t espera = 1088;
	DDRD = (1<<PORTD7);
	
	Inicializacion_Motor();
	
	sei();
	
	
	
// 	PORTB &= ~(1<<PORTB3);
// 	
// 	PORTB |= (1<<PORTB3);
// 	_delay_ms(2000);
// 	
// 	for(i=0;i<(500);i++){
// 		PORTB |=  (1<<PORTB2);
// 		_delay_us(espera);
// 		PORTB &=  ~(1<<PORTB2);
// 		_delay_us(espera);
// 	}
	
	_delay_ms(1000);
	buscarOrigen();
	irPos(13000);
	_delay_ms(100);
	setVelocidad(200);
	_delay_ms(1500);
	setVelocidad(0);
	_delay_ms(500);
	setVelocidad(-500);
	_delay_ms(1500);
	setVelocidad(0);
// 	int16_t i;
//  	for (i=1000;i>0;i-=10)
//  	{
//  		_delay_ms(20);
//  		setVelocidad(i);
//  	}
	
	
	
    while (1) 
    {
		
		
	/*	
		_delay_ms(100);
		PORTB |=(1<<PORTB3);
		_delay_ms(100);
		
		for(i=0;i<(180);i++){
			PORTB |=  (1<<PORTB2);
			_delay_us(espera);
			PORTB &=  ~(1<<PORTB2);
			_delay_us(espera);
		}
		
		_delay_ms(100);
		PORTB &=~(1<<PORTB3);		
		_delay_ms(100);
		
		for(i=0;i<(180);i++){
			PORTB |=  (1<<PORTB2);
			_delay_us(espera);
			PORTB &=  ~(1<<PORTB2);
			_delay_us(espera);
		}*/
    }
}