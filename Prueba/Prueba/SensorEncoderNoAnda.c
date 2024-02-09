/*
 * SensorEncoder.c
 *
 * Created: 8/1/2023 3:49:03 AM
 *  Author: Calvo
 */ 

#include "SensorEncoder.h"


#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

uint8_t static volatile iPosAng=0,angulosAnt[10]={0}, promAnt=0,mostrar=7;
int16_t static volatile angulo = 0,anguloAnt=0,velAng=0,velAngAnt=0,posAng[4]={0};
uint8_t static volatile IZQ=0,IZQAct=0,IZQAnt=0,DER=0,DERAct=0,DERAnt=0,HayDato=0;
//el tiempo medida es 20 us tiengo que poner un modulo 1000000
uint32_t static volatile tiempoMedida = 0,tiempo=0,tiempoMedidaAnt=0, dt=0;


typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;


void DescomponerDigitos(int16_t numero);
void DescomponerDigitosSinSigno(uint16_t);
void DescomponerDigitosSinSigno32(uint32_t);
void Timer0Init(void);
void set_origen();
void actualizar_MEF();
void actualizar_vel();

void SensorEncoder_init(){
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD |= (1<<PORTD2); //pongo B2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD |= (1<<PORTD3);//pongo B3 como pull up
	
	Timer0Init();
	
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
	OCR0A = 249;			//19+1
	TCCR0A = (1<<WGM01); //modo CTC
	TCCR0B |= (1 << CS01) | (1 << CS00);	//CTC CLK/64 =16MHz/64 =250KHz
	TIMSK0 = (1<<OCIE0A); // habilitamos interrpución COMPA
}

ISR(TIMER0_COMPA_vect){ //interrupción periódica de periodo Tisr=250/250kHz=1ms
	tiempo++;
	actualizar_vel();
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
		tiempoMedida = tiempo;
		anguloAnt = angulo;
		angulosAnt[iPosAng++] = angulo;
		
		//actualizar_vel();
	}
}

void actualizar_vel(){
	int16_t prom=0;
	uint8_t i;
	
	for(i=0;i<iPosAng;i++){
		prom = prom + angulosAnt[iPosAng];
	}
	
	if (iPosAng != 0)
	{
		prom = prom/iPosAng;
	}else{
		prom = promAnt+1;
		//velAng = 99;
	}
		
	
	velAng = prom - promAnt; //dividido 1 ms
	
	if (iPosAng!=0)
	{
		promAnt = prom;	
	}
		
	
	
	if(EspacioDisponibleBuffer()>33 && velAngAnt != velAng && mostrar>0){
		cli();
		mostrar--;
		velAngAnt = velAng;
		AgregarDatoBuffer('T');
		DescomponerDigitosSinSigno32(tiempo);
		AgregarDatoBuffer('A');
		DescomponerDigitos(angulo);
		
		//muestra
		AgregarDatoBuffer('V');
		DescomponerDigitos(velAng);
		AgregarDatoBuffer('d');
		DescomponerDigitosSinSigno(promAnt);
		AgregarDatoBuffer('\n');
		AgregarDatoBuffer('\0');
		sei();
	}
	if(mostrar==0 && EspacioDisponibleBuffer()>225){
		mostrar=7;
	}
	iPosAng = 0;
}


ISR(INT1_vect){ //interrupción periódica de periodo Tisr=40/2MHz=20us  o fisr=2MHz/40=500kHz
	//leo el puerto y actualizo
	IZQAct = LEERIZQ;
	DERAct = LEERDER;
	
	IZQ = IZQAct;	
	DER = DERAct;
	
//	AgregarDatoBuffer('I');
//	AgregarDatoBuffer('\0');
	
	actualizar_MEF();
	
	HayDato=1;
}

//sensor derecho
ISR(INT0_vect){
	IZQAct = LEERIZQ;
	DERAct = LEERDER;
	
	IZQ = IZQAct;
	DER = DERAct;
	
//	AgregarDatoBuffer('D');
//	AgregarDatoBuffer('\0');
	actualizar_MEF();
	
	HayDato=1;
}