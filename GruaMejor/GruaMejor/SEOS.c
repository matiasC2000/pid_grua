/*
Tanto el motor como el sensor son por interrupciones
el enviar datos tanbien es por interrupciones pero el agregar datos no

La SEOS es para calcular de forma periodica el PID y el mandar datos
ademas debe llevar el tiempo

Para informar la velocidad es de 9600 para mandar 40caractes tarda 4ms
por lo tanto informo cada 25 ms que es 40 veces por segundo

*/

#include "SEOS.h"
#include "PID.h"
#include <avr/io.h>
#include "SensorEncoder.h"
#include "Informar.h"


static unsigned char contInformo=0,contPID=1,contTiempo=0;			


volatile unsigned char Flag_Informo=0;
volatile unsigned char Flag_PID=0;
volatile uint32_t tiempoSEOS=0;

uint32_t getTiempoSEOS(){
	return tiempoSEOS;
}

void SEOS_SCH_Tasks (void)
{
	/*
	  llamo a las tareas que tengo que hacer
	*/
	if (++contPID==3) {
		Flag_PID=1;				//Tarea programada cada 3ms
		contPID=0;
	}
	

	if (++contInformo==3) {
		Flag_Informo=1;			//Tarea programada cada 3 ms
		contInformo=0;
	}}void SEOS_Dispatch_Tasks(void){	if(Flag_Informo){		Informar_Actulizar();		Flag_Informo = 0;	}	if(Flag_PID){		Actulizar_PID();		Flag_PID = 0;	}}//inicializador del sOESvoid SEOS_Init(){	/*		configurar el reloj
		el reloj tiene que funcionar cada 50ms, ya que es el tiempo mas chico
		como no llega a 50ms, cuento hasta 50 cada 1ms
		para tener mayor exactitud uso 1 ms	*/	OCR0A  = 249;			//249+1
	TCCR0A = (1<<COM0A0) | (1<<WGM01);		//modo CTC, Toggle on compare match
	TCCR0B = (1<<CS01)|(1<<CS00);			//CTC CLK/64 =16MHz/64 =250KHz prescalar en 64
	TIMSK0 = (1<<OCIE0A);					//habilitamos interrpución COMPA}//configurar la interrupción del clock//interrupción periódica cada 250/250kHz=1msISR(TIMER0_COMPA_vect){	SEOS_SCH_Tasks();	if(contTiempo++<10){		tiempoSEOS++;		contTiempo=0;	}}