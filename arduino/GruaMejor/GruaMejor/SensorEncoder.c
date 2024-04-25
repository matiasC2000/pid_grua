/*
 * SensorEncoder.c
 *
 * Created: 8/1/2023 3:49:03 AM
 *  Author: Calvo
 */ 


//360 -- 709 ------ -6 de error 700 valores 706  ------   -16 de error 696 valores 712
//1 vuelta son aprox 760
//da 1 vuelta por seg
//1 rendija cada 0.0013 ms
//frecuencia de muestreo de 0.0003s
//1/16000000*500 =0.0003
//64*x=500
//x = 7


#include "SensorEncoder.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include "SEOS.h"

#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

int16_t static volatile angulo = 0,derivate_counter_D=0,derivate_counter_I=0;
int16_t static anguloANt=0,signoAnt=0;
uint8_t static volatile IZQ=0,DER=0, count_equals=0;
uint32_t static volatile tiempoMuestra =0;

int16_t static derivate_D=0,derivate_I=0,axiAnt;

typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;

void f00(void);
void f10(void);
void f01(void);
void f11(void);

void (*MEF[])(void)={f00,f10,f01,f11};


void set_origen();


/*
calcular la derivada del angulo aca la longitud es fija pero el tiempo es el que varia
antes lo hacia al revez
que boludo

puedo calculos
calcular con un clock mucho mas mejor
*/

void SensorEncoder_init(){
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD |= (1<<PORTD2); //pongo B2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD |= (1<<PORTD3);//pongo B3 como pull up
	
	EIMSK |= (1<<INT0); //habilito INT0
	EICRA |= (1<<ISC00);
	EICRA &= ~(1<<ISC01);
	//puse a INT0 para activarse cuando sube y cuando baja
	
	EIMSK |= (1<<INT1);//habilito INT1
	EICRA |= (1<<ISC10);
	EICRA &= ~(1<<ISC11);
	//puse a INT1 para activarse cuando sube y cuando baja
	
	// Configuración del Timer 2 en modo CTC (Clear Timer on Compare Match)
	TCCR2A = (1 << WGM21);  // Configuración del Timer 2 en modo CTC
	TCCR2B = (1 << CS22);   // Prescaler de 256
	OCR2A = 4;             // Valor de comparación para contar hasta 4
	TIMSK2 = (1 << OCIE2A); // Habilitar la interrupción de comparación A
	
	set_origen();
}

//puede ser que tenga que agragar aca a un vector circular
//y pasar el vector circular y filtrarlo en el PID
//puede que le lleve much procesamiento y se haga muy lento porque lo tengoque hacer en la maquina de estado

void getAnguloEncoder(int16_t *anguloOUT, int16_t *tiempoMuestraOUT){
	*anguloOUT = angulo;
	if(tiempoMuestraOUT!=0){
		//*tiempoMuestraOUT = tiempoMuestra;
		int16_t axi = (derivate_D + derivate_I)/2;
		//puedo hacer que duvuelva el mismo y que no cambie
		
		//*tiempoMuestraOUT = anguloANt * axi;
		if(anguloANt == signoAnt){
			if(axiAnt != axi){
				axiAnt = (derivate_D + derivate_I)/2;
				*tiempoMuestraOUT = anguloANt * axiAnt;
				count_equals=0;
			}else{
				*tiempoMuestraOUT = anguloANt*((derivate_D + derivate_I)/2+derivate_counter_D/5);
				//*tiempoMuestraOUT = anguloANt * ((derivate_D + derivate_I)/2+(derivate_counter_D+derivate_counter_I)/2)/2;
			}
		}else{
			*tiempoMuestraOUT = 16000;
		}
		signoAnt= anguloANt;
	}
}

void set_origen(){
	if (DER & IZQ) estado = UnoUno;
	if ((!DER) & IZQ) estado = CeroUno;
	if (DER & (!IZQ)) estado = UnoCero;
	if ( (!DER) & (!IZQ)) estado = CeroCero;
	
	_delay_ms(50);
	IZQ = LEERIZQ;
	DER = LEERDER;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
	
	angulo = 0;	
}

//se toma el tiempo para saber cuando ocurrio la toma del dato
ISR(INT1_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//	leer entradas
	derivate_I = derivate_counter_I;
	derivate_counter_I = 0;
	IZQ = LEERIZQ;
	DER = LEERDER;
	//tiempoMuestra = tiempoSEOS;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
}

//sensor derecho
ISR(INT0_vect){
	//leer entradas
	derivate_D = derivate_counter_D;
	derivate_counter_D = 0;
	IZQ = LEERIZQ;
	DER = LEERDER;
	//tiempoMuestra = tiempoSEOS;
	(*MEF[estado])(); //ejecuta la funcion correspondiente
}

void f00(void){
	if(DER){
		angulo--;
		anguloANt=-1;
		estado = CeroUno;
	}
	if (IZQ){
		angulo++;
		anguloANt=1;
		estado = UnoCero;
	}
}
void f10(void){
	if(DER){
		angulo++;
		anguloANt=1;
		estado = UnoUno;
	}
	if(!IZQ){
		angulo--;
		anguloANt=-1;
		estado = CeroCero;
	}
}
void f01(void){
	if(!DER){
		angulo++;
		anguloANt=1;
		estado = CeroCero;
	}
	if(IZQ){
		angulo--;
		anguloANt=-1;
		estado = UnoUno;
	}
}
void f11(void){
	if(!DER){
		angulo--;
		anguloANt=-1;
		estado = UnoCero;
	}
	if(!IZQ){
		angulo++;
		anguloANt=1;
		estado = CeroUno;
	}
}

ISR(TIMER2_COMPA_vect) {
// 	derivate_counter_D=derivate_counter_D+derivate_counter_D==32000;
// 	derivate_counter_I=derivate_counter_I+derivate_counter_I==32000;
	if(derivate_counter_D!=3200)derivate_counter_D++;
	if(derivate_counter_I!=3200)derivate_counter_I++;
}