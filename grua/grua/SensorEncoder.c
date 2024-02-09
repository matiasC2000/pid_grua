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

#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

uint8_t static volatile iPosAng=4;
int16_t static volatile angulo = 0,prom = 0,promdt=0, anterior=0,anguloAnt=0,velAng=0,posAng[4]={0},dif,actualzacionDt=0,difAnt =0;
uint8_t static volatile IZQ=0,IZQAct=0,IZQAnt=0,DER=0,DERAct=0,DERAnt=0,HayDato=0;
//el tiempo medida es 20 us tiengo que poner un modulo 1000000
uint32_t static volatile tiempoMedida = 0,tiempoMedidaAnt=0, dt=0,tiempo=0;



typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;


void DescomponerDigitos(int16_t numero);
void DescomponerDigitosSinSigno(uint16_t);
void DescomponerDigitosSinSigno32(uint32_t);
void set_origen();
void actualizar_MEF();
void mostrarDatos();

void SensorEncoder_init(){
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD |= (1<<PORTD2); //pongo B2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD |= (1<<PORTD3);//pongo B3 como pull up
	
	/*//timer
	OCR2A= 6;	//39+1
	TCCR2A =(1<<WGM21)|(1<<WGM20); //modo CTC, Toggle on compare match
	TCCR2B =(1<<CS21);	//CTC CLK/64 =16MHz/64 = 250kHz
	TIMSK2 =(1<<OCIE2A); // habilitamos interrpución COMPA
	*/
	EIMSK |= (1<<INT0); //habilito INT0
	EICRA |= (1<<ISC00);
	EICRA &= ~(1<<ISC01);
	//puse a INT0 para activarse cuando sube y cuando baja
	
	EIMSK |= (1<<INT1);//habilito INT1
	EICRA |= (1<<ISC10);
	EICRA &= ~(1<<ISC11);
	//puse a INT1 para activarse cuando sube y cuando baja
	
	
	set_origen();
}


int16_t leerAnguloEncoder(){
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
}



void mostrarDatosEncoder(){
	if(EspacioDisponibleBuffer()>40){
		cli();
		//muestra
		AgregarDatoBuffer('T');
		DescomponerDigitosSinSigno32(tiempoMedida);
		AgregarDatoBuffer('A');
		DescomponerDigitos(angulo);
		
		AgregarDatoBuffer('V');
		DescomponerDigitos(velAng);
		AgregarDatoBuffer('d');
		DescomponerDigitosSinSigno(dt);
		
		AgregarDatoBuffer('\n');
		AgregarDatoBuffer('\0');
		sei();
	}
}

ISR(TIMER2_COMPA_vect){ //interrupción periódica de periodo Tisr=40/2MHz=1ms  o fisr=2MHz/40=500kHz
	IZQ = LEERIZQ;
	DER = LEERDER;
	
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
}

uint32_t SensorEncoder_getTiempo(){
	return tiempo;
}

ISR(INT1_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//leo el puerto y actualizo
	IZQ = LEERIZQ;
	DER = LEERDER;
	
	actualizar_MEF();
	
	HayDato=1;
}

//sensor derecho
ISR(INT0_vect){
	IZQ = LEERIZQ;
	DER = LEERDER;
	
	actualizar_MEF();
	
	HayDato=1;
}