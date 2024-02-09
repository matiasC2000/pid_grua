/*
 * SensorEncoder.c
 *
 * Created: 8/1/2023 3:49:03 AM
 *  Author: Calvo
 */ 

#include "SensorEncoder.h"


#define LEERIZQ PIND & (1<<PORTD3)
#define LEERDER PIND & (1<<PORTD2)

uint8_t static volatile iPosAng=4;
int16_t static volatile angulo = 0,prom = 0,promdt=0, anterior=0,anguloAnt=0,velAng=0,posAng[4]={0},dif,actualzacionDt=0,difAnt =0;
uint8_t static volatile IZQ=0,IZQAct=0,IZQAnt=0,DER=0,DERAct=0,DERAnt=0,HayDato=0;
//el tiempo medida es 20 us tiengo que poner un modulo 1000000
uint32_t static volatile tiempoMedida = 0,tiempo=0,tiempoMedidaAnt=0, dt=0;


typedef enum {CeroCero,UnoCero,CeroUno,UnoUno} state;
state static volatile estado;


void DescomponerDigitos(int16_t numero);
void DescomponerDigitosSinSigno(uint16_t);
void DescomponerDigitosSinSigno32(uint32_t);
void Timer1Init(void);
void set_origen();
void actualizar_MEF();
void actualizar_vel();
void calcularDt();
void calcularDtMasTiempo();

void SensorEncoder_init(){
	//PORTD2 es INT0 es sensor derecha un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD2); //pongo B2 como entrada
	PORTD |= (1<<PORTD2); //pongo B2 como pull up
	
	//PORTD3 es INT1 es sensor izquierda un 1 en el sensor es que esta tapado
	DDRD &= ~(1<<PORTD3);//pongo B3 como entrada
	PORTD |= (1<<PORTD3);//pongo B3 como pull up
	
	Timer1Init();
	
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

void Timer1Init(void){
	//configuración del TOPE del contador TCNT0
	OCR1A=19;			//19+1
	TCCR1A = (1<<WGM11); //modo CTC
	TCCR1B = (1<<CS11);	//CTC CLK/8 =16MHz/8 =2MHz
	TIMSK1 = (1<<OCIE1A); // habilitamos interrpución COMPA
}

ISR(TIMER1_COMPA_vect){ //interrupción periódica de periodo Tisr=20/2MHz=20us  o fisr=2MHz/20=10000kHz
	tiempo++;
	if(actualzacionDt++>500){
		actualzacionDt=0;
		tiempoMedida = tiempo;
		//calcularDt();
		calcularDtMasTiempo();
		mostrarDatos();
	}
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
	
		tiempoMedida = tiempo;
		
				
		actualizar_vel();
		anguloAnt = angulo;
}

void actualizar_vel(){
	
	/*
	prom = posAng[(iPosAng-3)%4]/8+ posAng[(iPosAng-2)%4]/8 + posAng[(iPosAng-1)%4]/4 + posAng[(iPosAng)%4]/2;
	velAng = (angulo-prom)*1000; //lo multiplico porque el dt seguro es grande y queda muy chuquito
	dt = tiempoMedida - tiempoMedidaAnt;
	velAng = velAng/dt;
	*/
	
	calcularDt();
	actualzacionDt = 0;
	
	
	//actulizo datos
	tiempoMedidaAnt = tiempoMedida;
	
	/*iPosAng++;
	if (iPosAng>160)iPosAng=4;
	posAng[iPosAng]=angulo;*/
	
	if (angulo != anguloAnt && EspacioDisponibleBuffer()>35)
	{
		mostrarDatos();
	}
	difAnt = dif;
	anterior = angulo;
}

int16_t leerVelocidad(){
	return velAng;
}

void calcularDtMasTiempo(){
	dt = tiempoMedida - tiempoMedidaAnt;
	promdt = promdt/2 + dt/2;
	//if(promdt <2){
		velAng = (dif*100000)/promdt;			//dif siempre da 1 es solo para darle signo a dtvelAng = promdt*(difAnt);
	//}else{
		//velAng =0;
	//}
	//velAng = (100000*(dif))/promdt;
}

void calcularDt(){
	dt = tiempoMedida - tiempoMedidaAnt;
	promdt = promdt/2 + dt/2;
	dif = angulo - anterior;
	//if(promdt <2){
		velAng = (dif*100000)/promdt;			//dif siempre da 1 es solo para darle signo a dtvelAng = promdt*(difAnt);
	//}else{
		//velAng =0;
	//}
	
	//velAng = (100000*(dif))/promdt;
}

void mostrarDatos(){
	if(EspacioDisponibleBuffer()>35){
		//muestra
		AgregarDatoBuffer('T');
		DescomponerDigitosSinSigno32(tiempoMedida);
		AgregarDatoBuffer('A');
		DescomponerDigitos(angulo);
		
		AgregarDatoBuffer('V');
		DescomponerDigitos(velAng);
		AgregarDatoBuffer('d');
		//DescomponerDigitos(anterior);
		DescomponerDigitosSinSigno(dt);
		
		AgregarDatoBuffer('\n');
		AgregarDatoBuffer('\0');
	}
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