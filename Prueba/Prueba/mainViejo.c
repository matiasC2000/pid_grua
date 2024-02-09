/*
 * Prueba.c
 *
 * Created: 7/20/2023 4:58:42 PM
 * Author : Calvo
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"

void DescomponerDigitos(int16_t numero);

//typedef enum{IGUALADOS,IZQUIERDA,DERECHA}state;
//CeroCero,UnoCero,CeroUno,UnoUno
typedef enum{CeroCero,UnoCero,CeroUno,UnoUno}state;
volatile state estado;


volatile int16_t angulo = 0;

volatile uint8_t izqValorAct=0, izqValorAnt=0, izq=0;
volatile uint8_t derValorAct=0, derValorAnt=0, der=0;

//Flags
volatile uint8_t HayDatoSensorOptico = 0,Der = 0,Izq = 0;



///
int main(void)
{
	Inicializacion_UART();
	DDRB |= (1<<PORTB1); //pongo PORTB1 como salida(luz)

	PORTB |= (1<<PORTB1);//prendo la luz
	_delay_ms(500);
	PORTB &= ~(1<<PORTB1);//apago la luz
	_delay_ms(500);
	
    //DDRB &= ~(1<<PORTB0); borrado
    //PORTB |= (1<<PORTB0);	borrado
	
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo D2 como entrada
	PORTD |= (1<<PORTD2); //pongo D2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo D3 como entrada
	PORTD |= (1<<PORTD3);//pongo D3 como pull up
	
	
	/*  Por int del cambio
	EIMSK |= (1<<INT0); //habilito INT0
	EICRA |= (1<<ISC00);
	EICRA &= ~(1<<ISC01);
	//puse a INT0 para activarse cuando sube y cuando baja
	
	EIMSK |= (1<<INT1);//habilito INT1
	EICRA |= (1<<ISC10);
	EICRA &= ~(1<<ISC11);
	//puse a INT1 para activarse cuando sube y cuando baja
	*/
	
	//TCCR0B =(1<<CS01);	//CTC CLK/8 =16MHz/8 =2Mhz
	//TIMSK0 |=(1<<TOIE0); // 2Mhz/256 = 7812.5
	
	OCR0A=40;			//39+1
	TCCR0A =(1<<COM0A0) | (1<<WGM01); //modo CTC, Toggle on compare match
	TCCR0B =(1<<CS01);	//CTC CLK/8 =16MHz/8 =2MHz
	TIMSK0 =(1<<OCIE0A); // habilitamos interrpución COMPA
	
	
	izqValorAnt = PIND&(1<<PORTD3);
	derValorAnt = PIND&(1<<PORTD2);
	
	sei();//habilita interrupciones
	
	if((!Izq) & (!Der)) estado = CeroCero;
	if((Izq) & (!Der)) estado = UnoCero;
	if((!Izq) & (Der)) estado = CeroUno;
	if(Izq&Der) estado = UnoUno;
	
	uint16_t mostrar = 0;
	AgregarDatosBuffer("Arranco");
	DescomponerDigitos(angulo);
	
	while (1) {
		DescomponerDigitos(angulo);
		if (estado == CeroCero)
		{
			if (Izq)
			{
				angulo++;
			}
			if(Der)
			{
				angulo--;
			}
		}
		if (estado == UnoCero)
		{
			if (!Izq)
			{
				angulo--;
			}
			if(Der)
			{
				angulo++;
			}
		}
			if (estado == CeroUno)
			{
				if (Izq)
				{
					angulo--;
				}
				if(!Der)
				{
					angulo++;
				}
			}
		if (estado == UnoUno)
		{
			if (!Izq)
			{
				angulo++;
			}
			if(!Der)
			{
				angulo--;
			}
		}
			
		if (angulo >0){
			PORTB |= (1<<PORTB1);//prendo la luz
		}else{
			PORTB &= ~(1<<PORTB1);//apago la luz
		}
	
}
}

ISR(TIMER0_COMPA_vect){
	
	derValorAct = PIND&(1<<PORTD2);
	izqValorAct = PIND&(1<<PORTD3);
	
	
	AgregarDatoBuffer('D');
	if(derValorAct){
		AgregarDatoBuffer('1');
		}else{
		AgregarDatoBuffer('0');
	}
	AgregarDatoBuffer('I');
	if(izqValorAct){
		AgregarDatoBuffer('1');
		}else{
		AgregarDatoBuffer('0');
	}
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\0');
	
	
	if(izqValorAct == izqValorAnt){
		if(izq++>3){
			Izq = izqValorAct;
			izq = 5;
		}
		}else{
		izqValorAnt = izqValorAct;
		izq=0;
	}
	
	if(derValorAct == derValorAnt){
		if(der++>3){
			Der = derValorAct;
			der = 5;
		}
		}else{
		derValorAnt = derValorAct;
		der=0;
	}
}


ISR(TIMER0_OVF_vect){
	//hago todo aca no es reomendable pero bueno
	//tiene que ser una tarea en el main es muy largo
	
	//esto se queda
	derValorAct = PIND&(1<<PORTD2);
	izqValorAct = PIND&(1<<PORTD3);
	
	
	AgregarDatoBuffer('D');
	if(derValorAct){
		AgregarDatoBuffer('1');
	}else{
		AgregarDatoBuffer('0');
	}
	AgregarDatoBuffer('I');
	if(izqValorAct){
		AgregarDatoBuffer('1');
		}else{
		AgregarDatoBuffer('0');
	}
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\0');
	
	
	if(izqValorAct == izqValorAnt){
		if(izq++>3){
			Izq = izqValorAct;
			izq = 5;
		}
	}else{
		izqValorAnt = izqValorAct;
		izq=0;
	}
	
	if(derValorAct == derValorAnt){
		if(der++>3){
			Der = derValorAct;
			der = 5;
		}
	}else{
		derValorAnt = derValorAct;
		der=0;
	}
	
	
	
	/*
	//miro el izq
	if(izqValorAct = izqValorAnt){
		//si leo el izq reseteo el der
		der = 0;
		if(izq++>3){//dijo que hay un cambio
			izq=0;
			izqValorAnt = izqValorAct;
			HayDatoSensorOptico = 1;
			Izq = 1;
		}
	}else{
		//con que una vez lo lea en 0 reseteo
		izq = 0;
	}
	
	//miro el der
	if (derValorAct != derValorAnt)//si hubo cambio
	{
		izq=0;
		if(der++>3){//dijo que hay un cambio
			der=0;
			derValorAnt = derValorAct;
			HayDatoSensorOptico = 1;
			Der = 1;
		}
	}else{
		der = 0;
	}
	*/
}


/*

//sensor izq 
ISR(INT1_vect){
	if (estado == IGUALADOS) {
		estado = IZQUIERDA;
		angulo--;
	}else{
		if (estado == DERECHA){
			estado = IGUALADOS;
		}else{
			estado = IGUALADOS;
			angulo++;
		}
	}
	AgregarDatosBuffer("I");
	DescomponerDigitos(angulo);
	AgregarDatosBuffer("\n");
}

//sensor derecho
ISR(INT0_vect){
	if (estado == IGUALADOS) {
		estado = DERECHA;
		angulo++;
		}else{
		if (estado == IZQUIERDA){
			estado = IGUALADOS;
			}else{
			estado = IGUALADOS;
			angulo--;
		}
	}
	AgregarDatosBuffer("D");
	DescomponerDigitos(angulo);
	AgregarDatosBuffer("\n");
}
	//if (PORTB&(1<<PORTB1)) {
		//PORTB &= ~(1<<PORTB1);
	//}else{
	//PORTB &= ~(1<<PORTB1);
	//}
	//PORTB = (~(PORTB&(1<<PORTB1)))|(PORTB|(1<<PORTB1));

/
int main(void)
{
	DDRB |= (1<<PORTB1);

	PORTB |= (1<<PORTB1);
	_delay_ms(500);
	PORTB &= ~(1<<PORTB1);
	_delay_ms(500);
	
	DDRB &= ~(1<<PORTB0);
	PORTB |= (1<<PORTB0);
	
	while (1)
	{
		if ((PINB & (1<<PORTB0)) ){
			
			PORTB |= (1<<PORTB1);
			}else{
			PORTB &= ~(1<<PORTB1);
		}
		
	}
	
}*/


/* prueba del led
DDRD |= (1<<PORTD7);

PORTD |= (1<<PORTD7);
_delay_ms(500);
PORTD &= ~(1<<PORTD7);
_delay_ms(500);

DDRD &= ~(1<<PORTD6);
PORTD |= (1<<PORTD6);
while (1)
{
	if (!(PIND & (1<<PORTD6))){
		PORTD |= (1<<PORTD7);
		}else{
		PORTD &= ~(1<<PORTD7);
	}
	_delay_ms(500);
}

*/









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