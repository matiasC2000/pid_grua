/*
 * sensor.c
 *
 * Created: 7/17/2023 11:54:58 AM
 *  Author: Calvo
 */ 

#include "sensor.h"

#define F_CPU 16000000UL
#include <util/delay.h>


int16_t volatile valor=500;

uint16_t hacerMedicion();

int16_t leerDato(){

	//bloqueante pero despues lo hago con int.
	//cada vez que lo leo tengo que hacer los calculos de la velocidad
	//hacer la velocidad independiente de esto
		
	ADCSRA |= (1<<ADSC);//start conversion
	while((ADCSRA&(1<<ADIF))==0);//wait for conversion to finish
	ADCSRA |= (1<<ADIF); //borrar flag
	
	//int16_t result =ADC; //resultado completo de 10 bits
	
	
	//sensor = pend×angulo + ordenada
	int16_t result = (ADC-ord);
	result = result/(-4.3234);
	
	return result;
}

uint16_t dameOrdenada(){
	return ord;
}

uint16_t hacerMedicion(){
	ADCSRA |= (1<<ADSC);//start conversion
	while((ADCSRA&(1<<ADIF))==0);//wait for conversion to finish
	ADCSRA |= (1<<ADIF); //borrar flag
	
	return ADC;
}

void Inicializacion_Sensor_Int(){
	DDRC &= ~(1<<PORTC0);
	PORTC &= ~(1<<PORTC0);
	
	DIDR0= 0x01; //Digital Input Disable (opcional)
	//ADCSRA= 0x87;//make ADC enable and select ck/128
	ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//el nuevo clk a 125 y con Interrup
	ADMUX |= (1<<REFS0);// Vref=AVCC, right-justified, ADC0 pin
}


int16_t leerDatoInt(){
	//sensor = pend×angulo + ordenada
// 	int16_t result = (valor-ord);
// 	result = result/(-4.3234);
	
	return valor;
}


//el Clock del ACD es 125KHz, con auto-trigger son 13.5 ciclos del ADCLK tiene una frecuencia de 9.2592KHz
//tiempo de muestra es de 108 micro 
ISR(ADC_vect)
{
	// Aquí puedes agregar el código que deseas que se ejecute cuando se complete la conversión ADC.
	valor = ADC;
}



void Inicializacion_Sensor(){
	DDRC &= ~(1<<PORTC0);
	PORTC &= ~(1<<PORTC0);
	
	DIDR0= 0x01; //Digital Input Disable (opcional)
	ADCSRA= 0x87;//make ADC enable and select ck/128
	ADMUX |= (1<<REFS0);// Vref=AVCC, right-justified, ADC0 pin	
	
	
	
	//calcula la ordenada o el punto mas bajo del pendulo
	ords[0]=hacerMedicion();
	_delay_ms(100);
	ords[1]=hacerMedicion();
	_delay_ms(100);
	ords[2]=hacerMedicion();
	
	ord = (ords[0]+ords[1]+ords[2])/3;
	
}