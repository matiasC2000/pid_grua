/*
 * SensorEncoder.c
 *
 * Created: 8/1/2023 3:49:03 AM
 *  Author: Calvo
 */ 

#include "SensorEncoder.h"


#define LEERIZQ PINB & (1<<PORTB3)
#define LEERDER PINB & (1<<PORTB2)


int16_t static volatile angulo = 0,anguloAnt=0;
uint8_t static volatile IZQ=0,IZQAct=0,IZQAnt=0,DER=0,DERAct=0,DERAnt=0,HayDato=0;
//el tiempo medida es 20 us tiengo que poner un modulo 1000000
uint16_t static volatile tiempoMedida = 0;

typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;


void DescomponerDigitos(int16_t numero);
void DescomponerDigitosSinSigno(uint16_t);
void Timer0Init(void);
void set_origen();
void actualizar_MEF();


void SensorEncoder_init(){
	//PORTB2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRB &= ~(1<<PORTB2); //pongo B2 como entrada
	PORTB |= (1<<PORTB2); //pongo B2 como pull up
	
	//PORTB3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRB &= ~(1<<PORTB3);//pongo B3 como entrada
	PORTB |= (1<<PORTB3);//pongo B3 como pull up
	
	Timer0Init();
	
	set_origen();
}

int16_t leerAngulo(){
	return angulo;
}

uint8_t hayDato(){
	uint8_t copia = HayDato;
	HayDato = 0;
	return copia;
}

void set_origen(){
	if (DER & IZQ) estado = UnoUno;
	if ((!DER) & IZQ) estado = CeroUno;
	if (DER & (!IZQ)) estado = UnoCero;
	if ( (!DER) & (!IZQ)) estado = CeroCero;
}

void Timer0Init(void){
	//configuración del TOPE del contador TCNT0
	OCR0A=39;			//39+1
	TCCR0A = (1<<WGM01); //modo CTC
	TCCR0B = (1<<CS01);	//CTC CLK/8 =16MHz/8 =2MHz
	TIMSK0 = (1<<OCIE0A); // habilitamos interrpución COMPA
}

ISR(TIMER0_COMPA_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//leo el puerto y actualizo
	if(tiempoMedida++>50000) tiempoMedida=0;
	
	IZQAct = LEERIZQ;
	DERAct = LEERDER;
	
	IZQ = IZQAct;	
	DER = DERAct;
	
	if(IZQAct != IZQAnt){
		IZQAnt = IZQAct;
		HayDato=1;
		//AgregarDatoBuffer('I');
		//AgregarDatoBuffer('\0');
	}
	if(DERAct != DERAnt){
		DERAnt = DERAct;
		HayDato=1;
		//AgregarDatoBuffer('D');
		//AgregarDatoBuffer('\0');
	}
	if (HayDato) actualizar_MEF();
}


void actualizar_MEF(){
	switch(estado){
		case CeroCero:
		if(DER){
			angulo--;
			estado = CeroUno;
		}
		if (IZQ){
			angulo++;
			estado = UnoCero;
		}
		break;
		
		case  UnoCero:
		if(DER){
			angulo++;
			estado = UnoUno;
		}
		if(!IZQ){
			angulo--;
			estado = CeroCero;
		}
		break;
		
		case CeroUno:
		if(!DER){
			angulo++;
			estado = CeroCero;
		}
		if(IZQ){
			angulo--;
			estado = UnoUno;
		}
		break;
		
		case UnoUno:
		if(!DER){
			angulo--;
			estado = UnoCero;
		}
		if(!IZQ){
			angulo++;
			estado = CeroUno;
		}
		break;
	}
	if (angulo != anguloAnt)
	{
		anguloAnt = angulo;
		AgregarDatoBuffer('T');
		DescomponerDigitosSinSigno(tiempoMedida);
		AgregarDatoBuffer('A');
		DescomponerDigitos(angulo);
		AgregarDatosBuffer('\n');
	}
}