/*
 * SensorEncoder.c
 *
 * Created: 8/1/2023 3:49:03 AM
 *  Author: Calvo
 */ 


//360 -- 709 ------ -6 de error 700 valores 706  ------   -16 de error 696 valores 712
//1 vuelta son aprox 760
//da 1 vuelta por seg
//1 rendija cada 0.0013 ms
//frecuencia de muestreo de 0.0003s
//1/16000000*500 =0.0003
//64*x=500
//x = 7


#include "SensorEncoder.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include "SEOS.h"

#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

int16_t static volatile angulo = 0;
uint8_t static volatile IZQ=0,DER=0;
uint32_t static volatile tiempoMuestra =0;

typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;

void f00(void);
void f10(void);
void f01(void);
void f11(void);

void (*MEF[])(void)={f00,f10,f01,f11};


void set_origen();

void SensorEncoder_init(){
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD |= (1<<PORTD2); //pongo B2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD |= (1<<PORTD3);//pongo B3 como pull up
	
	EIMSK |= (1<<INT0); //habilito INT0
	EICRA |= (1<<ISC00);
	EICRA &= ~(1<<ISC01);
	//puse a INT0 para activarse cuando sube y cuando baja
	
	EIMSK |= (1<<INT1);//habilito INT1
	EICRA |= (1<<ISC10);
	EICRA &= ~(1<<ISC11);
	//puse a INT1 para activarse cuando sube y cuando baja
	
	set_origen();
}


void getAnguloEncoder(int16_t *anguloOUT, uint32_t *tiempoMuestraOUT){
	*anguloOUT = angulo;
	if(tiempoMuestraOUT!=0){
		*tiempoMuestraOUT = tiempoMuestra;
	}
}

void set_origen(){
	if (DER & IZQ) estado = UnoUno;
	if ((!DER) & IZQ) estado = CeroUno;
	if (DER & (!IZQ)) estado = UnoCero;
	if ( (!DER) & (!IZQ)) estado = CeroCero;
	
	_delay_ms(50);
	IZQ = LEERIZQ;
	DER = LEERDER;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
	
	angulo = 0;	
}

//se toma el tiempo para saber cuando ocurrio la toma del dato
ISR(INT1_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//	leer entradas
	IZQ = LEERIZQ;
	DER = LEERDER;
	tiempoMuestra = tiempoSEOS;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
}

//sensor derecho
ISR(INT0_vect){
	//leer entradas
	IZQ = LEERIZQ;
	DER = LEERDER;
	tiempoMuestra = tiempoSEOS;
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