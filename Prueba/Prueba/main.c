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
#include "SensorEncoder.h"


// #define LEERIZQ PIND & (1<<PORTD3)
// #define LEERDER PIND & (1<<PORTD2)
// 
// 
// uint8_t volatile IZQ=0,IZQAct=0,IZQAnt=0,DER=0,DERAct=0,DERAnt=0,HayDato=0;
// 
// typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
// state estado;
// 
void DescomponerDigitos(int16_t numero);
void Timer0Init(void);

int main(void)
{
	Inicializacion_UART();
	DDRB |= (1<<PORTB1); //pongo PORTB1 como salida(luz)

	PORTB |= (1<<PORTB1);//prendo la luz
	_delay_ms(500);
	PORTB &= ~(1<<PORTB1);//apago la luz
	_delay_ms(500);
	
	int16_t angulo = 0,anguloAnt=0;
// 	uint16_t mostrar = 0;
// 	
// 	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
// 	DDRD &= ~(1<<PORTD2); //pongo D2 como entrada
// 	PORTD |= (1<<PORTD2); //pongo D2 como pull up
// 	
// 	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
// 	DDRD &= ~(1<<PORTD3);//pongo D3 como entrada
// 	PORTD |= (1<<PORTD3);//pongo D3 como pull up
// 	
// 	Timer0Init();
	SensorEncoder_init();
	
	sei();
	
	angulo = leerAngulo();
	
	AgregarDatosBuffer("Hola");
	DescomponerDigitos(angulo);
	_delay_ms(500);
	
// 	if (DER & IZQ) estado = UnoUno;
// 	if ((!DER) & IZQ) estado = CeroUno;
// 	if (DER & (!IZQ)) estado = UnoCero;
// 	if ( (!DER) & (!IZQ)) estado = CeroCero;
	
	
	while (1) {
		
// 		switch(estado){
// 			case CeroCero:
// 			if(DER){
// 				 angulo--;
// 				 estado = CeroUno;
// 			}
// 			if (IZQ){
// 				angulo++;
// 				estado = UnoCero;
// 			}
// 			break;
// 		
// 			case  UnoCero:
// 			if(DER){
// 				angulo++;
// 				estado = UnoUno;
// 			}
// 			if(!IZQ){
// 				angulo--;
// 				estado = CeroCero;
// 			}
// 			break;
// 			
// 			case CeroUno:
// 			if(!DER){
// 				angulo++;
// 				estado = CeroCero;
// 			}
// 			if(IZQ){
// 				angulo--;
// 				estado = UnoUno;
// 			}
// 			break;
// 			
// 			case UnoUno:
// 			if(!DER){
// 				angulo--;
// 				estado = UnoCero;
// 			}
// 			if(!IZQ){
// 				angulo++;
// 				estado = CeroUno;
// 			}
// 			break;
// 		}
// 		if(anguloAnt != angulo){
// 			anguloAnt = angulo;
// 			DescomponerDigitos(angulo);
// 		}
		
// 		while(IZQ){
// 			
// 		}
// 		angulo++;
// 		DescomponerDigitos(angulo);
// 		while(! IZQ){
// 			
// 		}
// 		angulo++;
// 		DescomponerDigitos(angulo);
		
		if(hayDato()){
			//AgregarDatoBuffer('a');
			//AgregarDatoBuffer('\0');
		}
		
		angulo = leerAngulo();
		if (angulo != anguloAnt)
		{
			anguloAnt = angulo;
			//DescomponerDigitos(angulo);
		}
		
		if (angulo >0){
			PORTB &= ~(1<<PORTB1);//apago la luz
		}else{
			PORTB |= (1<<PORTB1);//prendo la luz
		}
	
	}
}

/*
void Timer0Init(void){
	//configuración del TOPE del contador TCNT0
	OCR0A=39;			//39+1
	TCCR0A = (1<<WGM01); //modo CTC
	TCCR0B = (1<<CS01);	//CTC CLK/8 =16MHz/8 =2MHz
	TIMSK0 = (1<<OCIE0A); // habilitamos interrpución COMPA
}

ISR(TIMER0_COMPA_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//leo el puerto y actualizo
	IZQAct = LEERIZQ;
	DERAct = LEERDER;
	IZQ = IZQAct;
	
	DER = DERAct;
	
	if(IZQAct != IZQAnt){
		IZQAnt = IZQAct;
		HayDato=1;
		AgregarDatoBuffer('I');
		AgregarDatoBuffer('\0');
	}
	if(DERAct != DERAnt){
		DERAnt = DERAct;
		HayDato=1;
		AgregarDatoBuffer('D');
		AgregarDatoBuffer('\0');
	}
}
*/

//de la mano de  mi amigo ChatGPT
// Función para descomponer el número en dígitos
void DescomponerDigitos(int16_t numero)
{
	if(numero<0) {
		numero = (-1)*numero;
		AgregarDatoBuffer('-');
	}else{
		AgregarDatoBuffer('+');
	}
	uint16_t divisor = 10000;
	int8_t digito;
	int16_t resto = numero;
	
	while (divisor >= 1)
	{
		digito = resto / divisor;
		resto = resto % divisor;
		divisor = divisor / 10;

		AgregarDatoBuffer(digito+48);
	}
	//AgregarDatoBuffer('\r');
	//AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\0');
}

void DescomponerDigitosSinSigno32(uint32_t numero){
	uint32_t divisor = 1000000000;
	int8_t digito;
	uint32_t resto = numero;

	while (divisor >= 1)
	{
		digito = resto / divisor;
		resto = resto % divisor;
		divisor = divisor / 10;

		AgregarDatoBuffer(digito+48);
	}
	//AgregarDatoBuffer('\r');
	//AgregarDatoBuffer('\n');
	AgregarDatoBuffer('\0');
}

void DescomponerDigitosSinSigno(uint16_t numero){
uint16_t divisor = 10000;
int8_t digito;
uint16_t resto = numero;

while (divisor >= 1)
{
	digito = resto / divisor;
	resto = resto % divisor;
	divisor = divisor / 10;

	AgregarDatoBuffer(digito+48);
}
//AgregarDatoBuffer('\r');
//AgregarDatoBuffer('\n');
AgregarDatoBuffer('\0');
}