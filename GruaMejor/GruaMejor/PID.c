/*
* PID.c
*
* Created: 8/2/2023 6:24:35 PM
*  Author: Calvo
*/


#include <avr/io.h>
#include "PID.h"
#include "UART.h"
#include "SensorEncoder.h"
#include "motor.h"
#include "SEOS.h"
#include "slideResistor.h"

#define ZONAMUERTA 0
#define ZONAMUERTAI 10000

//ki 4 Kp-170

int16_t static eant = 0, s = 0,posRef;

//Para derivada y posRef
//int16_t static Kd = 6,Kp = 3, Ki=0;
//Para posRef solo Kp
//int16_t static Kd = 0,Kp = 3, Ki=0;
//Para derivada, integlal y posRef
//int16_t static Kd = 6,Kp = 3, Ki=1;

//quieto en un punto y para que suba
//int16_t static Kd = 0,Kp = 3, Ki=0;

int16_t static Kd = 0,Kp = -20, Ki=0;

//quieto en un punto y hace pendulo
//int16_t static Kd = 0,Kp = 3, Ki=0;

int16_t static derivada=0,vel,e,tiempoDev;
uint32_t static tiempoMuestra=0;
uint8_t vecesIgual=1;
int16_t posAnt[2]={0,0};
uint32_t static tiempoAnt[2]={0,0};
int16_t sen2[]={0,7,15,23,31,39,47,54,62,70,78,86,94,101,109,117,125,133,140,148,156,164,171,179,187,195,202,210,218,225,233,241,248,256,263,271,278,286,294,301,309,316,323,331,338,346,353,360,368,375,382,389,397,404,411,418,425,432,439,446,453,460,467,474,481,488,495,502,509,515,522,529,535,542,549,555,562,568,575,581,587,594,600,606,612,619,625,631,637,643,649,655,661,667,673,678,684,690,695,701,707,712,718,723,728,734,739,744,750,755,760,765,770,775,780,785,790,794,799,804,809,813,818,822,827,831,835,840,844,848,852,856,860,864,868,872,876,880,883,887,891,894,898,901,904,908,911,914,917,920,923,926,929,932,935,938,940,943,946,948,951,953,955,958,960,962,964,966,968,970,972,974,975,977,979,980,982,983,985,986,987,988,990,991,992,993,993,994,995,996,996,997,998,998,998,999,999,999,999,999};
// 0.45      0.90      1.35		1.8		 2.25	  2.7	  3.15	   3.6		4.05	 4.5
// 0.007853  0.015707  0.023559	0.031410 0.039259 0.04710 0.054950 0.062790 0.070626 0.078459                           0.078459     

void calcularDerivada(int16_t);
void calcularIntegral(int16_t);

void Actulizar_PID(){
	getAnguloEncoder(&e,&tiempoMuestra);
	
	
	e= e%800;
	if(e<0){
		e = -e;
		e = e%400;
		if(e>200){
			e = 200-(e%200);
		}
		e = - sen2[e];
	}else{
		e = e%400;
		if(e>200){
			e = 200-(e%200);
		}
		e = sen2[e];
	}
	
	//el palo mide 6500 pasos de la grua
	//para hacer el error la pocicion de la carga respecto de el inicio del carrito
	//hago regla de 3	999---->6500
	//					x  ---->x*6500/1000= x*6.5
	//e = 7000-getPos()-e*6.5;//+
	
	//cambiar este fragmento por
	// 1024 ----> 13616   x----> x*13616/1024 = x*13.3
	posRef = getSlideResistor()*13.3;
	
	//Para derivada y posRef
	e = e*6.5;
	// y el error para la derivada es sin el posRef y capaz un cambio de signo
	
	calcularDerivada(e-getPos());
	
	//Para derivada y posRef
//	e = posRef-getPos()+e;		//e = posRef-getPos()+e*6.5; lo hago por partes
	
	//quieto en un punto y para que suba
 	posRef = 7000;
// 	e = posRef-getPos()-e;
	
	calcularIntegral(e);
		
	vel = (Kp*e)/10;// + (Kd*derivada)/10 + Ki*s/100;
	
	//capar la velocidad para que no se derborde
	//la velocidad maxima es de 1000
	if(vel>1000) vel=1000;
	if(vel<-1000) vel=-1000;
	setVelocidad(vel);
}

int16_t getDerivadaPID(){
	return derivada;
}

int16_t getTiempoDev(){
	return tiempoDev;
}

int16_t getVelocidadPID(){
	return vel;
}

int16_t getIntegralPID(){
	return s;
}

int16_t getError(){
	return e;
}

void calcularIntegral(int16_t e){
	s += e/7;
	if(s>ZONAMUERTAI) s = ZONAMUERTAI;
	if(s<-ZONAMUERTAI) s = -ZONAMUERTAI;
}

void calcularDerivada(int16_t e){

	if(getTiempoSEOS()>tiempoAnt[0]+60){
		if(e == posAnt[0]){
			derivada = (e-posAnt[1])*100;		  //maximo 47
			tiempoDev = (getTiempoSEOS()-tiempoAnt[1]);//suele ser <20 cuando se mueve cayendo y 50 cuando va lento pero se mueve
			derivada = derivada/tiempoDev;
		}
		if(e != posAnt[0]){
			derivada = (e-posAnt[0]);//*100;		  //maximo 47
			tiempoDev = (tiempoMuestra-tiempoAnt[0]);//suele ser <20 cuando se mueve cayendo y 50 cuando va lento pero se mueve
			derivada = derivada;///tiempoDev;
			
			posAnt[1]=posAnt[0];
			tiempoAnt[1]=tiempoAnt[0];
			posAnt[0]=e;
			tiempoAnt[0]=tiempoMuestra;
		}
	}
	
// 	if(e == posAnt[0]){
// 		derivada = (posAnt[0]-posAnt[1])/(tiempo-tiempoAnt[1]);
// 		tiempoAnt[1] = (tiempoAnt[0]-tiempoAnt[1])/2;
// 	}else{
// 		tiempoAnt[1]=tiempoAnt[0];
// 		posAnt[1]=posAnt[0];
// 		derivada=(e-posAnt[0])/(tiempo-tiempoAnt[0]);
// 		posAnt[0]=e;
// 		tiempoAnt[0]=tiempo;
// 	}
}