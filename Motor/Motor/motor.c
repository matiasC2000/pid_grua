/*
 * motor.c
 *
 * Created: 7/17/2023 5:15:46 PM
 *  Author: Calvo
 */ 


#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define FIN 13616
#define IRIZQ PORTB &= ~(1<<PORTB1)
#define IRDER PORTB |= (1<<PORTB1)

// 	MS1 MS2 MS3
// 	L	L	L	full
// 	H	L	L	1/2
// 	L	H	L	1/4
// 	H	H	L	1/8
// 	H	H	H	1/16
// #define PASOS1  PORTC = 0b11111;			//PASOS de 1(1/16)
// #define PASOS2  PORTC = 0b11011;			//PASOS de 2(1/8)
// #define PASOS4  PORTC = 0b11010;			//PASOS de 4(1/4)
// #define PASOS8  PORTC = 0b11001;			//PASOS de 8(1/2)
// #define PASOS16 PORTC = 0b11000;			//PASOS de 16(1)

#define PASOS1  PORTB = (PORTB & 0b000111)|0b111000;			//PASOS de 1(1/16)
#define PASOS2  PORTB = (PORTB & 0b000111)|0b011000;			//PASOS de 2(1/8)
#define PASOS4  PORTB = (PORTB & 0b000111)|0b010000;			//PASOS de 4(1/4)
#define PASOS8  PORTB = (PORTB & 0b000111)|0b001000;			//PASOS de 8(1/2)
#define PASOS16 PORTB = (PORTB & 0b000111)|0b000000;			//PASOS de 16(1)


void darPaso(int8_t);

static volatile uint16_t pos=0;
static volatile int8_t lado=0,salto=1;
// 1 es derecha -1 es izquierd


void setVelocidad(int16_t velocidad){
	uint16_t espera = velocidad;
	//velocidad negativa va a izq
	if(velocidad<0) {lado=-1; velocidad = (-1)*velocidad;IRIZQ;}
	else {lado = 1;IRDER;}
	if(velocidad == 0){ espera = 64000;
						PASOS1;salto=1;
	}else 
		if(velocidad>500){
			espera = 30000/velocidad;
			PASOS16;salto=16;
		}
		else {
			if(velocidad>250){
				espera = 15000/velocidad;
				PASOS8;salto=8;
			}else{
				if (velocidad>125)
				{
					espera = 7500/velocidad;
					PASOS4;salto=4;
				}else{
					if(velocidad>63){
						espera = 3750/velocidad;
						PASOS2;salto=2;
					}else{
						espera = 1875/velocidad;
						PASOS1;salto=1;
					}
				}
			}
			
		}
	
	//el tiempo de espera minimo esta 33 pero se puede mas hacer pruebas
	//velocidad maxima de 1000
	//tiempo espera = 33000/velociadad
	
	if(espera-1 < TCNT1){
		OCR1A = espera;
		TCNT1 = espera-2;
	}else{
		OCR1A = espera;
	}
}

void Inicializacion_Motor(){
	//pongo como salida
	DDRB |= (1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5);
	DDRD |= (1<<PORTD7);
	
	//pongo como salida los pines que controlan el tamaio de paso
	DDRC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2);
	
	TCCR1A =  0;
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10); //modo CTC y preescaler de 1024
	TIFR1 = (1<<OCF1B)|(1<<OCF1A);
	
	//timer
	OCR1B = 16;			//16+1
	OCR1A = 6000;			//32+1 velocidad maxima
	
	 // Habilitar interrupciones
	TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B);
}

uint16_t getPos(){
	return pos;
}

void irPos(uint16_t destino){
	PASOS16;salto=16;
	_delay_ms(100);
	if(destino>0 && destino<FIN){
		if(pos<destino)
			while(pos<destino){
				darPaso(1);pos += lado*salto;
			}
		else
			while(pos>destino){
				darPaso(-1);pos += lado*salto;
			}
	}
}

//-1 izq z der
void darPaso(int8_t sentido){
	if (sentido < 0){
		IRIZQ;
		lado = -1;
	}
	else{
		IRDER;
		lado = 1;
	}
	PORTB |=  (1<<PORTB2); //pongo en 1 el esclon de step
	_delay_ms(3);
	PORTB &=  ~(1<<PORTB2); //pongo en 0 el escalon de step
	_delay_ms(3);
}


void buscarOrigen(){
	//configurar el puerto de entrada pin6 D6
	DDRD &= ~(1<<PORTD6);
	//colocar el pin con pull up
	PORTD |= (1<<PORTD6);
	//prendo el led
	PORTD |= (1<<PORTD7);
	
	_delay_ms(500);			//espero un poco porque el pinta salir corriendo a veces
	PASOS16;salto=16;
	while((PIND & (1<<PORTD6))){	//mientras no toque el boton
		darPaso(-1);
	}
	//encuntra el inicio de a pasos muy grandes, lo vuelvo a buscar con pasos mas chiquitos
	
	pos=0;
	irPos(50);
	_delay_ms(500);//espero porque los pasos son tan rapidos que sigo precionando el boton
	
	PASOS1;salto=1;
	while((PIND & (1<<PORTD6))){	//mientras no toque el boton
		darPaso(-1);
	}
	
	PASOS16;salto=16;
	//apago el led
	PORTD &= ~(1<<PORTD7);
	pos = 0;
}


ISR(TIMER1_COMPA_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	if ( (pos+lado)<FIN && (pos+lado)>0)
	{
		PORTB |=  (1<<PORTB2); //pongo en 1 el esclon de step
		pos += lado*salto;
	}		
}

ISR(TIMER1_COMPB_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	PORTB &=  ~(1<<PORTB2); //pongo en 0 el escalon de step
}

