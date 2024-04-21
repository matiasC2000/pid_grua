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
#include "button.h"
#include "leerDatos.h"

#define MULTIPLICADOR_MS 50


static unsigned char contInformo=0,contPID=1,contButtton=3,contTiempo=0,contLeerDatos=5;			


volatile unsigned char Flag_Informo=0,Flag_PID=0,Flag_Button=0,Flag_leerDatos=0;
volatile uint32_t tiempoSEOS=0,tiempoInforme=0;

uint32_t getTiempoSEOS(){
	return tiempoInforme;
}

void SEOS_SCH_Tasks (void)
{
	/*
	  llamo a las tareas que tengo que hacer
	*/
	if (++contPID==1) {
		Flag_PID=1;				//Tarea programada cada 3ms
		contPID=0;
	}
	if (++contInformo==3*MULTIPLICADOR_MS) {
		Flag_Informo=1;			//Tarea programada cada 3 ms
		contInformo=0;
	}	if (++contButtton==5*MULTIPLICADOR_MS) {
		Flag_Button=1;			//Tarea programada cada 5 ms
		contButtton=0;
	}	if (++contLeerDatos==20*MULTIPLICADOR_MS) {
		Flag_leerDatos=1;			//Tarea programada cada 20 ms
		contLeerDatos=0;
	}}void SEOS_Dispatch_Tasks(void){	if(Flag_Informo){		Informar_Actulizar();		Flag_Informo = 0;	}	if(Flag_PID){		Actulizar_PID();		Flag_PID = 0;	}	if(Flag_Button){		button_Actulizar();		Flag_Button = 0;	}	if(Flag_leerDatos){		actulizar_leerDatos();		Flag_leerDatos=0;	}}//inicializador del sOESvoid SEOS_Init(){	/*		configurar el reloj
		el reloj tiene que funcionar cada 50ms, ya que es el tiempo mas chico
		como no llega a 50ms, cuento hasta 50 cada 1ms
		para tener mayor exactitud uso 1 ms	*/	TCCR0A = (1 << WGM01);  // Modo CTC
	TCCR0B = (1 << CS01);   // Prescaler de 8
	OCR0A = 39;
	TIMSK0 = (1<<OCIE0A);					//habilitamos interrpución COMPA}//configurar la interrupción del clock//interrupción periódica cada 50kHz= 0.05msISR(TIMER0_COMPA_vect){	SEOS_SCH_Tasks();	tiempoSEOS++;	if(contTiempo++>MULTIPLICADOR_MS){		tiempoInforme++;	//cuenta ms		contTiempo=0;	}}