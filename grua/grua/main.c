/*
 * grua.c
 *
 * Created: 7/12/2023 8:28:16 PM
 * Author : Calvo
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#include "main.h"

#include <math.h>


#include "SensorEncoder.h"
#include "UART.h"
#include "motor.h"
#include "PID.h"

void DescomponerDigitos(int16_t);

int16_t lectura,velocidad,error=0;
uint16_t i=0,objetivo=1700/2;

extern uint8_t volatile FLAG_PID;
extern int16_t velocidad;


int main(void)
{
	SensorEncoder_init();
	//Inicializacion_Motor();	
	Inicializacion_UART();
	sei();
	
	setVelocidad(0);
	
	//DescomponerDigitos(dameOrdenada());
	
	//buscarOrigen();	
	
	AgregarDatosBuffer("Prueba derivada \n");
	_delay_ms(500);
	//irPos(1700/2);
	
	velocidad = 0;
	//setVelocidad();
	
	while(1){
		i++;
		if (FLAG_PID == 1)
		{
			FLAG_PID = 0;
			
			//usa el tiempo del sensor que es de 1/10000 = 0.0001 seg = 0.1 ms
			lectura = leerAnguloEncoder();
			//error = sin(0.0086*lectura)*2850;//largo 
			//error = (error) + (objetivo - getPos());
			velocidad = Actulizar_PID(lectura, lectura);
			//error = objetivo - getPos();
			//velocidad = Actulizar_PIDDoble(lectura,error);
			//setVelocidad();
		}
	}
}

/*

//de la mano de  mi amigo ChatGPT
// Función para descomponer el número en dígitos
void DescomponerDigitos(int16_t numero)
{
	if(numero<0) {
		numero = (-1)*numero;
		AgregarDatoBuffer('-');
	}
	
	uint16_t divisor = 1000;
	int8_t digito;
	int16_t resto = numero;
	
	while (divisor >= 1)
	{
		digito = resto / divisor;
		resto = resto % divisor;
		divisor = divisor / 10;

		AgregarDatoBuffer(digito+48);
	}
	AgregarDatoBuffer('\r');
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\0');
}
*/