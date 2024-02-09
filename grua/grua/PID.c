/*
* PID.c
*
* Created: 8/2/2023 6:24:35 PM
*  Author: Calvo
*/


#include <avr/io.h>
#include "PID.h"
#include "UART.h"

#define ZONAMUERTA 0
#define Ki 0

//ki 4 Kp-170


void mostrarDatosPID(int16_t a,int16_t e);

int16_t static eant = 0,  correcion = 0 ,tant = 0, s = 0,eantInt=0, tCero=0;
int16_t static Kd = 0,Kp = 170;
int16_t static derivada;
int16_t static diff;
int16_t static vel;
uint32_t tiempo=0,esperaCero=5000;
int16_t static altura;
uint8_t esperar = 1;

int16_t static aAnt=0,derivadaA=0,KpA=15,KdA=2,kpE=0;

void calcularDerivada(int16_t);
void calcularIntegral(int16_t);

int16_t Actulizar_PIDDoble(int16_t a,int16_t e){
	//no uso t porque es siempre uno
	//para la derivada capaz combiene un prom
	tiempo++;
	
	a = a + correcion;
	
	//con 1000 anda para hacer que el angulo se corriga pero tiene mucho delay
	//tomo medidas cada 1000, pero segun calculos parece ser que es mejor cada 220
	if(tiempo%25 == 0){
		derivadaA = a - aAnt;	//capaz tiene que ser de 2
		
		if(derivadaA == 0)
		{
			tCero = tCero + 25;
			if(tCero > esperaCero){
				//correcion = -a;
				s = 0;
				if(esperar){
					irPos(1700/2+5);
					esperar = 0;
				}
				esperaCero = 50000;
			}
			}else{
			tCero = 0;
		}
		aAnt = a;
	}
	
	if ( a > -ZONAMUERTA && a < ZONAMUERTA)
	{
		a = 0;
		}else{
		if(a> ZONAMUERTA) a = a-ZONAMUERTA;
		if(a<-ZONAMUERTA) a = a+ZONAMUERTA;
	}
	mostrarDatosPID(a,e);
	
	vel = KpA*a+ KdA*derivadaA+kpE*e;
	if(esperar){
		vel = 0;
		correcion = -a;
		PORTD |= (1<<PORTD7);//prendo led
		}else{
		PORTD &= ~(1<<PORTD7);//apago led
	}
	if(vel>20000) vel=20000;
	if(vel<-20000) vel=-20000;
	return vel;
}



int16_t Actulizar_PID(int16_t a,int16_t e){
	//no uso t porque es siempre uno
	//para la derivada capaz combiene un prom
	tiempo++;
	
	//e = e + correcion;
	
	//con 1000 anda para hacer que el angulo se corriga pero tiene mucho delay
	if(tiempo%25 == 0){		//tomo medidas cada 1000, pero segun calculos parece ser que es mejor cada 220
		derivada = e - eant;	//capaz tiene que ser de 2
		calcularIntegral(e);
		
		if(derivada == 0)
		{
			tCero = tCero + 25;
			if(tCero > esperaCero){
				correcion = -a;
				s = 0;
				if(esperar){
					irPos(1700/2+5);
					esperar = 0;
				}
				esperaCero = 3100;
			}
			}else{
			tCero = 0;
		}
		eant = e;
	}
	
	//calcularIntegral(e);
	/*
	if (a > 600 || a < 100){
	esperaCero = 400000;
	esperar = 1;
	PORTD |= (1<<PORTD7);//prendo led
	}*/
	if (e>-ZONAMUERTA && e<ZONAMUERTA)
	{
		e = 0;
		}else{
		if(e>ZONAMUERTA)e = e-ZONAMUERTA;
		if(e<-ZONAMUERTA)e = e+ZONAMUERTA;
	}
	
	vel = Kp*e+ Kd*derivada;//+Ki*s;
	if(esperar){
		vel = 0;
		PORTD |= (1<<PORTD7);//prendo led
		}else{
		PORTD &= ~(1<<PORTD7);//apago led
	}
	mostrarDatosPID(a,e);
	if(vel>20000) vel=20000;
	if(vel<-20000) vel=-20000;
	return vel;
}

void calcularIntegral(int16_t e){
	/*if (e>-10 && e<10)
	{
	e = 0;
	}else{
	if(e>10)e = e-10;
	if(e<-10)e = e+10;
	
	}*/
	//diff = e - eantInt;
	//altura = e + diff/2;
	s += e;
	if(s>5) s = 5;
	if(s<-5) s = -5;
	eantInt = e;
}

void calcularDerivada(int16_t e){
	derivada = e - eant;
	if(eant != e) eant = e;
}


void mostrarDatosPID(int16_t a,int16_t e){
	if(EspacioDisponibleBuffer()>45){
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
		DescomponerDigitos(getPos());
		
		AgregarDatoBuffer('\n');
		AgregarDatoBuffer('\0');
	}
}
