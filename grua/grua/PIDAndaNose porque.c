/*
 * PID.c
 *
 * Created: 8/2/2023 6:24:35 PM
 *  Author: Calvo
 */ 


#include <avr/io.h>
#include "PID.h"
#include "UART.h"

void mostrarDatosPID(int16_t a,int16_t e);

int16_t static eant = 0, tant = 0, s = 0;
int16_t static Ki = 1,Kp = 20;
int16_t static derivada;
int16_t static diff;
int16_t static vel;
uint32_t tiempo=0;
int16_t static altura;

void calcularDerivada(int16_t);
void calcularIntegral(int16_t);

int16_t Actulizar_PID(int16_t a,int16_t e){
	//no uso t porque es siempre uno
	//para la derivada capaz combiene un prom
	tiempo++;
	
	calcularDerivada(e);
	
	
	//e = -e;
	if (e>-2 && e<2)
	{
		e = 0;
		}else{
		if(e>2)e = e-2;
		if(e<-2)e = e+2;
	}
	vel = Kp*e;// + Ki*s;
	mostrarDatosPID(a,e);
	
		
	eant = e;	
	return vel;
}

void calcularIntegral(int16_t e){
	diff = e - eant;
	altura = e + diff/2;
	s += altura;
}

void calcularDerivada(int16_t e){
	derivada = e - eant;
//	eant = e;
}


void mostrarDatosPID(int16_t a,int16_t e){
	if(EspacioDisponibleBuffer()>45){
		cli();
		//muestra
		AgregarDatoBuffer('T');
		DescomponerDigitosSinSigno32(tiempo);
		AgregarDatoBuffer('E');
		DescomponerDigitos(e);
		
		AgregarDatoBuffer('A');
		DescomponerDigitos(a);
		
		AgregarDatoBuffer('V');
		DescomponerDigitos(vel);
		AgregarDatoBuffer('d');
		//DescomponerDigitos(anterior);
		DescomponerDigitos(derivada);
		
		AgregarDatoBuffer('\n');
		AgregarDatoBuffer('\0');
		sei();
	}
}
