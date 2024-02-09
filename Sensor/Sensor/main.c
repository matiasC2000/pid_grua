/*
 * Sensor.c
 *
 * Created: 10/21/2023 2:13:32 PM
 * Author : Calvo
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

int16_t static volatile angulo = 0,anguloIni=0;
uint8_t static volatile IZQ=0,DER=0,DERANT=0,IZQANT=0;

typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;

void f00(void);
void f10(void);
void f01(void);
void f11(void);

void (*MEF[])(void)={f00,f10,f01,f11};
state volatile static estado;

int main(void)
{
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD &= ~(1<<PORTD2); //pongo B2 como pull up
	DDRB |= (1 << PORTB0);  // Configura el pin D7 como salida
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD &= ~(1<<PORTD3);//pongo B3 como pull up
	
	DDRB |= (1 << PORTB0);  // Configura el pin D7 como salida
	
	if (DER & IZQ) estado = UnoUno;
	if ((!DER) & IZQ) estado = CeroUno;
	if (DER & (!IZQ)) estado = UnoCero;
	if ( (!DER) & (!IZQ)) estado = CeroCero;

	EIMSK |= (1<<INT0); //habilito INT0
	EICRA |= (1<<ISC00);
	EICRA &= ~(1<<ISC01);
	//puse a INT0 para activarse cuando sube y cuando baja
	
	EIMSK |= (1<<INT1);//habilito INT1
	EICRA |= (1<<ISC10);
	EICRA &= ~(1<<ISC11);
	//puse a INT1 para activarse cuando sube y cuando baja
	
	angulo = 0;
 	
	sei();
	
	_delay_ms(50);
	IZQ = LEERIZQ;
	DER = LEERDER;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
	
	_delay_ms(50);
	anguloIni = angulo;
	
    while (1) 
    {
		if(angulo == anguloIni){
			PORTB |= (1 << PORTB0);
		}else{
			PORTB &= ~(1 << PORTB0);
		}
		_delay_ms(20);
    }
}

ISR(INT1_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
//	leer entradas
	IZQ = LEERIZQ;
	DER = LEERDER;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
		
}

//sensor derecho
ISR(INT0_vect){
	//leer entradas
 	IZQ = LEERIZQ;
 	DER = LEERDER;
 	(*MEF[estado])(); //ejecuta la funcion correspondiente
	
}

void f00(void){
	if(DER){
		angulo--;
		estado = CeroUno;
	}
	if (IZQ){
		angulo++;
		estado = UnoCero;
	}
}
void f10(void){
	if(DER){
		angulo++;
		estado = UnoUno;
	}
	if(!IZQ){
		angulo--;
		estado = CeroCero;
	}
}
void f01(void){
	if(!DER){
		angulo++;
		estado = CeroCero;
	}
	if(IZQ){
		angulo--;
		estado = UnoUno;
	}
}
void f11(void){
	if(!DER){
		angulo--;
		estado = UnoCero;
	}
	if(!IZQ){
		angulo++;
		estado = CeroUno;
	}
}