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
#include "global.h"
#include <math.h>


#define CANT_DERIVATE 11

#define ZONAMUERTA 0
#define ZONAMUERTAI 10000

#define LIMITESUPI 1500
//arriba 1700
//pos 700

//ki 4 Kp-170

//Para derivada y posRef
//float static Kd = 6/700,Kp = 3, Ki=0;
float static Kd = 0,Kp = 1, Ki=0;
//Para posRef solo Kp
//int16_t static Kd = 0,Kp = 3, Ki=0;
//Para derivada, integlal y posRef
//int16_t static Kd = 6,Kp = 3, Ki=1/700;

//quieto en un punto y para que suba
//int16_t static Kd = 0,Kp = 3, Ki=0;

//float static Kd = 0,Kp = 20, Ki=0;

//quieto en un punto y hace pendulo
//int16_t static Kd = 0,Kp = 3, Ki=0;

uint8_t FLAG_habilitar_PID=1,i_derivative=0,cambio=1,FLAG_habilitar_manual=1;
uint8_t estado=0,countStop=0,fueMayor100=0,noFueCero=1,enPosicion=0;
float static floate=0,vel,velAnt=0, s = 0,posRef, derivada=0,derivada_encoder=0,derivada_angulo=0,derivada_pos=0,derivadaAnt=0;
uint16_t posRefManual;
float derivadas[CANT_DERIVATE];
float tiempoMuestraSoft=32000;
int16_t static e,tiempoDev,valor, eant = 0;;
int16_t static tiempoMuestra=0;
uint8_t vecesIgual=1,indice_ec_error = 1;
int16_t puntosAngulo;
int8_t direccion=0;

enum Indices_ec {
	EC_BASICO,
	EC_SIN_SEN,
	EC_POS,
	EC_INTERVERTIDO
};
// Definición de tipos de funciones de error
typedef float (*EcuacionesError)();
typedef float (*State)();

int16_t posAnt[2]={0,0};
uint32_t static tAnt=0;
uint32_t static tiempoAnt[2]={0,0};
int16_t sen2[]={0,7,15,23,31,39,47,54,62,70,78,86,94,101,109,117,125,133,140,148,156,164,171,179,187,195,202,210,218,225,233,241,248,256,263,271,278,286,294,301,309,316,323,331,338,346,353,360,368,375,382,389,397,404,411,418,425,432,439,446,453,460,467,474,481,488,495,502,509,515,522,529,535,542,549,555,562,568,575,581,587,594,600,606,612,619,625,631,637,643,649,655,661,667,673,678,684,690,695,701,707,712,718,723,728,734,739,744,750,755,760,765,770,775,780,785,790,794,799,804,809,813,818,822,827,831,835,840,844,848,852,856,860,864,868,872,876,880,883,887,891,894,898,901,904,908,911,914,917,920,923,926,929,932,935,938,940,943,946,948,951,953,955,958,960,962,964,966,968,970,972,974,975,977,979,980,982,983,985,986,987,988,990,991,992,993,993,994,995,996,996,997,998,998,998,999,999,999,999,999,1000};
// 0.45      0.90      1.35		1.8		 2.25	  2.7	  3.15	   3.6		4.05	 4.5
// 0.007853  0.015707  0.023559	0.031410 0.039259 0.04710 0.054950 0.062790 0.070626 0.078459                           0.078459

void calcularDerivada_encoder();
void calcularDerivada_angulo(int16_t);
void calcularDerivada_pos(int16_t);

void calcularIntegral(int16_t);

float ec_basico();
float ec_sin_sen();
float ec_pos();

float levantar();
float superArriba();
float frenoAbajo();

State stateBalance[] = {levantar,superArriba,frenoAbajo};

EcuacionesError calcularError[] = {ec_basico,ec_sin_sen,ec_pos};

void Actulizar_PID(){
	//capar la velocidad para que no se derborde
	//la velocidad maxima es de 1000
	
	if (FLAG_button_A_change)
	{
		if(FLAG_habilitar_PID){
			FLAG_habilitar_PID = 0;
			}else{
			FLAG_habilitar_PID = 1;
		}
		FLAG_button_A_change=0;
	}
	

	if (FLAG_button_B_change)
	{
		estado=0;
		fueMayor100=0;

		// 			if(FLAG_habilitar_manual){
		// 				FLAG_habilitar_manual = 0;
		// 			}else{
		// 				FLAG_habilitar_manual = 1;
		// 			}
		FLAG_button_B_change=0;
	}

	
	//	vel = getSlideResistor()*13.3-getPos();
	
	
	
	
	
	
	//esto tendria que andar hacer test
	// 	if(FLAG_habilitar_manual){
	// 		posRefManual = getSlideResistor()*13.3;
	// 		if(getPos() != posRefManual){
	// 			if(posRefManual<getPos()){
	// 				vel = vel-100;
	// 			}else{
	// 				vel = vel+100;
	// 			}
	// 		}else{
	// 			vel = 0;
	// 		}
	// 		if(vel>1700) vel=1700;
	// 		if(vel<-1700) vel=-1700;
	// 	}else{
	getAnguloEncoder(&e,&tiempoMuestra);
	
	vel = calcularError[indice_ec_error]();
	
	// no me gusta este pedazo de codigo
	//tengo que poner un limite de acceleracion al codigo para que no patine el motor el motor
	//float velMax = vel*0.30;
	//
	// 	if (velMax>1700)
	// 	{
	// 		velMax = 1700;
	// 	}
	// 	if (velMax<-1700)
	// 	{
	// 		velMax = -1700;
	// 	}
	if(vel>1500) vel=1500;
	if(vel<-1500) vel=-1500;
	if(FLAG_habilitar_PID){
		noFueCero=1;
		if(indice_ec_error==EC_POS){
			if(enPosicion==0){
				vel=0;
				posRef = getSlideResistor()*13.3;
				if(getPos()>(uint16_t)posRef-100 && getPos()<(uint16_t)posRef+100){
					enPosicion=1;
					ONLEDGREEN;
				}
			}	
		}else{
			ONLEDGREEN;
		}
	}else{
		if(indice_ec_error==EC_POS){
			//modo velocidad manual
			enPosicion=0;
			posRef = getSlideResistor();
			valor = getSlideResistor();
			if(noFueCero==0){
				if(posRef<485){
					vel = 4*(posRef - 485);
					}else{
					if(posRef>515){
						vel = 4*(posRef-515);
						}else{
						vel = 0;
					}
				}
				}else{
				if(posRef>485 && posRef<515){
					noFueCero=0;
				}
				vel = 0;
			}
			OFFLEDGREEN;
		}else{
			OFFLEDGREEN;
			vel=0;
		}
		
	}
	
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

int16_t getValor(){
	return valor;
}

void calcularIntegral(int16_t e){
	s += Ki*e;
	if(s > LIMITESUPI) s = LIMITESUPI;
	if(s < -LIMITESUPI) s = -LIMITESUPI;
}


void calcularDerivada_encoder(){
	if(tiempoMuestra < 29000){
		derivada_encoder = (15625/tiempoMuestra)*0.02+derivada_encoder*0.98;	//(20000/tiempoMuestra)*0.02+derivada_encoder*0.98;	
	}else{
		derivada_encoder = 0+derivada_encoder*0.98;	//(20000/tiempoMuestra)*0.02+derivada_encoder*0.98;	
	}
}

void setKi(float val){
	Ki=val;
}
void setKd(float val){
	Kd=val;
}
void setKp(float val){
	Kp=val;
}

void setIndice_ec_error(uint8_t indice){
	if (indice != indice_ec_error)
	{
		indice_ec_error = indice;
		s = 0; //reseteo el valor de la suma de la integral se resetea
	}
}

int16_t calcularSen(int16_t sen){
	sen= sen%800;
	if(sen<0){
		sen = 800+sen;
	}
	if(sen>399){
		sen = sen%400;
		if(sen>200) sen = 200-(sen%200);
		sen = - sen2[sen];
		}else{
		sen = sen%400;
		if(sen>200) sen = 200-(sen%200);
		sen = sen2[sen];
	}
	return sen;
}

void calcularDerivada_angulo(int16_t e){
	calcularDerivada_encoder();
	derivada_angulo = calcularSen(e+200)*(derivada_encoder/1000)*(float)51.05088;
	//la derivada de la posicion tiene que ser
	//calcularSen(e+200)*0.00255*(derivada_encoder/1000);
}

float ec_basicoREAL(){
	int8_t multi=1;
	puntosAngulo = e;
	e = e % 800;
	if(e<0){
		e = 800+e;
	}
	if(e > 250 && e < 550){
		multi = -1;
		Kp=1;
		}else{
		Kp=0;
	}
	e=calcularSen(e);
	valor = e;
	calcularDerivada_angulo(puntosAngulo);
	calcularIntegral(e);
	derivada = derivada_angulo;
	
	return( (multi*Kp)*e + Kd*derivada + s);
}

float levantar() {
	float vel;
	int16_t e_original = e;
	puntosAngulo = e;
	e = e % 800;  // Mantener e en el rango de -799 a

	if (e < 0) {
		e = 800 + e;  // Ajustar e para que esté en el rango de 0 a 799
	}

	if (fueMayor100==1) {
		if (e > 372 && e < 428) {
			// Cambio de estado arriba porque está arriba
			estado = 1;
			s = 0;
			fueMayor100 = 0;
			if (e > 400) {
				vel = -500;
				} else {
				vel = 500;
			}
		}
		calcularDerivada_angulo(puntosAngulo);
		derivada = derivada_angulo;
		e = calcularSen(e);
		valor = e;
		vel = -0.8 * e; // +derivada*20; // -5 30 -3.5 15
	}
	if (fueMayor100==0) {
		e_original = abs(e_original)+400; //le sumo 400 porque es media vuelta
		if (e_original%800 > 500) {  // Comprobación si la magnitud de puntosAngulo supera 100
			fueMayor100 = 1;
		}
		vel = 1.50 * calcularSen(((int16_t) (((float)getTiempoSEOS())/(2.42)) )%800);
	}
	
	return vel;
}



float superArriba(){
	puntosAngulo = e;
	e = e % 800;
	valor = e;
	if(e<0){
		e = 800+e;
	}
	e = e+1;
	if( !(e > 350 && e < 450)){
		estado =2;
		vel=0;
	}
	
	calcularDerivada_angulo(puntosAngulo);
	derivada = derivada_angulo;
	e=calcularSen(e);
	calcularIntegral(e);
	valor=e;
	vel =Kp*e+s+Kd*derivada;//30 0.2	//28	3
	/*
	e = abs(e)%800 - 400;

	if( !(e < 20 && e > -20)){
	estado =2;
	vel=0;
	}

	if(e>0){
	calcularIntegral(1);
	return(Kp*sqrt(e)+Ki*s);
	}else{
	if(e<0){
	e = (-1)*e;
	calcularIntegral(-1);
	return(Kp*sqrt(e)*(-1)+Ki*s);
	}
	}
	
	// 	if(e>0){
	// 		calcularIntegral(1);
	// 		return((float)-400*sqrt(e)+Ki*s);
	// 		}else{
	// 		if(e<0){
	// 			e = (-1)*e;
	// 			calcularIntegral(-1);
	// 			return((float)-400*sqrt(e)*(-1)+Ki*s);
	// 		}
	// 	}
	*/
}

float frenoAbajo(){
	puntosAngulo = e;
	e=calcularSen(e);
	
	//aca cambio de estado lo tengo que pasar a subir arriba
	if(e>-20 && e<20){
		countStop++;
		if (countStop>50){
			countStop=0;
			fueMayor100=0;
			estado=0;
		}
		}else{
		countStop=0;
	}
	
	e = e*6.5;	//el lagro es: 20
	valor = getPos()+e;
	
	posRef = 7500;
	//Para derivada y posRef
	e = posRef-getPos()+e;		//e = posRef-getPos()+e*6.5; lo hago por partes
	
	derivada = derivada_pos;
	calcularIntegral(e);
	
	return(0.7*e);
}

float ec_basico(){
	//State stateBalance[] = {levantar,superArriba,frenoAbajo};
	return(stateBalance[estado]());
}

float ec_sin_sen(){
	
	/*
	//hago que vuelva
	e= e;//%800;
	valor = e;
	
	calcularDerivada_encoder(e);
	calcularIntegral(e);
	derivada = derivada_encoder;
	*/

//	e = abs(e+400)%800 - 400;
	//  	if(e<0){
	//  		e = 800+e;
	//  	}

	floate = e*6.28/800;
	valor = e;
	
	if(floate>0){
		calcularIntegral(1);
		return(Kp*sqrt(floate)+s);
	}else{
		if(floate<0){
			floate = (-1)*floate;
			calcularIntegral(-1);
			return(Kp*sqrt(floate)*(-1)+s);
	 	}else{
			 return 0;
		 }
	 }
	//return( Kp*e + Kd*derivada + Ki*s);
}

void calcularDerivada_pos(int16_t e){
	//antes de esto hacer lo de mandar mensajes
	//falta poner que cos(a)*derivada del angulo - vel
	//con eso tendria que andar 10 puntos
	calcularDerivada_angulo(e);
	//vel + derivada_angulo*6.5;
	float velAxi=vel;
	if (vel>1700)
	{
		velAxi=1700;
	}
	if (vel<-1700)
	{
		velAxi=-1700;
	}
	derivada_pos = derivada_angulo - velAxi*(8.0645);
	//int16_t calculoAxi = e - eant;
	//eant = e;
	//derivada = (float)(0.02)*(float)calculoAxi+(float)(0.98)*derivada;
}


float ec_pos(){
	puntosAngulo = e;
	e=calcularSen(e);
	
	//el palo mide 6500 pasos de la grua
	//para hacer el error la pocicion de la carga respecto de el inicio del carrito
	//hago regla de 3	999---->6500
	//					x  ---->x*6500/1000= x*6.5
	//e = 7000-getPos()-e*6.5;//+
	
	//cambiar este fragmento por
	// 1024 ----> 13616   x----> x*13616/1024 = x*13.3
	posRef = getSlideResistor()*13.3;
	
	//Para derivada y posRef
	e = e*6.5;	//el lagro es: 20
	valor = getPos()+e;
	// y el error para la derivada es sin el posRef y capaz un cambio de signo
	
	
	calcularDerivada_pos(puntosAngulo);		//por ahora dejo fuera la pos
	
	//Para derivada y posRef
	e = posRef-getPos()+e;		//e = posRef-getPos()+e*6.5; lo hago por partes
	
	//quieto en un punto y para que suba
	//posRef = 7000;
	//e = posRef-getPos()-e;
	
	derivada = derivada_pos;
	
	calcularIntegral(e);
	return( Kp*e + Kd*derivada + s );	//0.3   0.07
}