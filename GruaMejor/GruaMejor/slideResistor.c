/*
 * slideResistor.c
 *
 * Created: 11/17/2023 9:27:11 PM
 *  Author: Calvo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define ZONAMUERTA 50

uint16_t static volatile valor=0;
uint16_t static valorAnt=0;
//el Clock del ACD es 125KHz, con auto-trigger son 13.5 ciclos del ADCLK tiene una frecuencia de 9.2592KHz
//tiempo de muestra es de 108 micro
ISR(ADC_vect)
{
	// Aquí puedes agregar el código que deseas que se ejecute cuando se complete la conversión ADC.
	valor = ADC;
}



void Inicializacion_slideResistor(){
	DDRC &= ~(1<<PORTC0);
	PORTC &= ~(1<<PORTC0);
	
	DIDR0= 0x01; //Digital Input Disable (opcional)
	//ADCSRA= 0x87;//make ADC enable and select ck/128
	ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//el nuevo clk a 125 y con Interrup
	ADMUX |= (1<<REFS0);// Vref=AVCC, right-justified, ADC0 pin
	ADCSRA |= (1<<ADSC);//start conversion
}

uint16_t hacerMedicion(){
	ADCSRA |= (1<<ADSC);//start conversion
	while((ADCSRA&(1<<ADIF))==0);//wait for conversion to finish
	ADCSRA |= (1<<ADIF); //borrar flag
	
	return ADC;
}

uint16_t getSlideResistor(){
	//getSliderResistor necesita una zona muerta para actualizarce porque tiene muchi ruido
	if(valor>valorAnt+ZONAMUERTA || valor<valorAnt-ZONAMUERTA){
		valorAnt = valor;
	}
	return valorAnt;
}