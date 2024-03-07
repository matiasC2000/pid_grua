/*
 * GruaMejor.c
 *
 * Created: 10/19/2023 1:03:26 PM
 * Author : Calvo
 */ 

#include <avr/io.h>
#include "UART.h"
#include <avr/interrupt.h>
#include "Informar.h"
#include "motor.h"
#include "SensorEncoder.h"
#include "slideResistor.h"
#include "SEOS.h"
#define F_CPU 16000000UL
#include <util/delay.h>

//prendo el led
#define ONLED PORTD |= (1<<PORTD5)
#define OFFLED PORTD &= ~(1<<PORTD5)

int16_t angulo,anguloAnt,pos;
int main(void)
{	
	uint8_t i=0;
	UART_Init();
 	SEOS_Init();
	Inicializacion_Motor();
	SensorEncoder_init();
	Inicializacion_slideResistor();
	
	DDRD |= (1 << PORTD5);  // Configura el pin D7 como salida para el led
	
	sei(); //habilito las interrupciones
	
	
	_delay_ms(50);
	Informar_MensajeInicial();
	
	setVelocidad(0);
	buscarOrigen();
	
	irPos(7000);
	
	ONLED;
	
	getAnguloEncoder(&anguloAnt, 0);
	

	//Espera hasta que obtenga un lectura constante
	while(i<3){
		_delay_ms(2000);
		getAnguloEncoder(&angulo, 0);
		if (anguloAnt == angulo)
		{
			i++;
		}else{
			anguloAnt = angulo;
			i=0;
		}
	}
	
	//setea el centro de pendulo
	set_origen();
	OFFLED;
	
	//se mueve hacia un lado para inciar la oscilacion
	irPos(7000+16*5);


	while (1) 
    {
		//llama a la maquina de estados para inciar el control
		SEOS_Dispatch_Tasks();
    }
}
