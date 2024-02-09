/*
 * motor.c
 *
 * Created: 7/17/2023 5:15:46 PM
 *  Author: Calvo
 */ 


#include "motor.h"
#include "UART.h"

#define ESCRIBIRMOTOR(bits) PORTB&=0b11000011; PORTB|= (bits&0b00111100)

//Definicion de limites de los pasos
#define FIN 1700
//124
#define VALOR_OCRA 124
//cambiando la variable no cambia pero es para los calculos
#define PRESCALAR 64
//MULTIPLICADOR_MS por tiempo ms te da el tiempo interno del prosesador
#define MULTIPLICADOR_MS (16000/((VALOR_OCRA+1)*PRESCALAR))
//ultimo numero tiempo en milisegundos
//por ahora el tiempo es 2ms pero pueder ser mas 2.5 o 3
#define TIEMPO_MINIMO_ENTRE_PASO MULTIPLICADOR_MS*2.5

static uint16_t inicio = 0;
//velocidad es pocicion actual = pos + vel * tiempo
int16_t velocidad=0;				//velocidad va a ser publica
uint8_t volatile FLAG_PID=0;		//se lee desde afuera, indica cuando calcular la velocidad en el PID
//pos es el posicion actual del carrrito, timerInt cada cuanto tengo que dar un paso, countTimer cuenta el tiempo entre pasos
static volatile uint16_t pos=0,timerInt=0,countTimer=0;
static volatile int8_t lado=0;



uint8_t mov[]={0b00001100,0b00011000,0b00110000,0b00100100};
uint8_t movLower[]={0b00000100,0b00001100 ,0b00001000,0b00011000 ,0b00010000, 0b00110000,0b00100000,0b00100100};


//vel en pasos por segundo
//la velocidad no puede ser mayor a 1000
void setVelocidad(){
	//de termino si tengo que sumar o restar a la posicion y hago modulo de la velocidad
	if(velocidad<0) {lado=-1; velocidad = (-1)*velocidad;}
	else lado = 1;
	
	
	//si la velociad es 0 el tiempo entre paso es cero para que no sea infinito
	if(velocidad==0)timerInt=0;
	else {
		if(velocidad>1000) velocidad=1000;
		timerInt = (1000*MULTIPLICADOR_MS)/velocidad;
	}
	
	
	/*
	if(velocidad<0) {lado=-1; velocidad = (-1)*velocidad;}
	else lado=1;
	if(velocidad==0)timerInt=0;
	else {
		//aca lo que paso si la velocidad es mayor a 40 no avansa porque la div da 0
		if (velocidad>20) velocidad = 20;
		cli();
		timerInt = 21.4/velocidad;//0.0429 // 0.0214
		sei();
	}
	*/
}

void irCentro(){
	irPos(FIN/2);
}

uint16_t getPos(){
	return pos;
}

void irPos(uint16_t posNueva){
	if(posNueva<FIN){
		while(pos<posNueva){
			pos++;
			ESCRIBIRMOTOR(movLower[pos%8]);
			_delay_ms(2);
		}
	}
}

void moverPasos(int16_t posNueva){
	uint16_t i=0;
	int8_t paso=1;
	
	if(posNueva<0){
		paso=-1;
		posNueva = posNueva*(-1);
	}else{
		for(i=pos%8;i<posNueva;i++){
			//PORTD = mov[i%4];
			pos+=paso;
			ESCRIBIRMOTOR(movLower[i%8]);
			_delay_ms(2);
		}
	}	
}

void irPosRelativa(){
	
}

void movimientoSexy(int16_t posNueva){
	//DDRD |=(1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5);
	
	//uint8_t movLower[]={0b00000100,0b00001100 ,0b00001000,0b00011000 ,0b00010000, 0b00110000,0b00100000,0b00100100};
	
	pos=0;
	while((PIND & (1<<PORTD6))){	//mientras no toque el boton
		ESCRIBIRMOTOR(movLower[pos%8]);
		pos++;
		_delay_ms(7);
	}
	
	/*uint16_t i=0,posAnterior=pos;
	int8_t paso=1;
	
	if(posNueva<0){
		paso=-1;
		posNueva = posNueva*(-1);
	}
	
	
	for(i=0;i<posNueva;i++){
		//PORTD = mov[i%4];
		pos+=paso;
		ESCRIBIRMOTOR(mov[pos%4]);
		_delay_ms(5);
	}*/
}

void Inicializacion_Motor(){
	DDRB |=(1<<PORTB2)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5);
	PORTB = mov[0];
	velocidad=0;
	lado=0;
	/*
	//timer
	OCR0A= VALOR_OCRA;//249;			//39+1
	TCCR0A =(1<<WGM01); //modo CTC, Toggle on compare match
	TCCR0B =(1<<CS01)|(1<<CS00);	//CTC CLK/64 =16MHz/64 = 250kHz
	TIMSK0 =(1<<OCIE0A); // habilitamos interrpución COMPA
	*/
	//inicio del FIN de carrera
	DDRD |= (1<<PORTD7);
	PORTD |= (1<<PORTD7);
	_delay_ms(500);
	PORTD &= ~(1<<PORTD7);
	_delay_ms(500);

	DDRD &= ~(1<<PORTD6);
	PORTD |= (1<<PORTD6);
}


ISR(TIMER0_COMPA_vect){ //interrupción periódica de periodo Tisr=40/2MHz=1ms  o fisr=2MHz/40=500kHz
	//lo llama cada 0.5ms
	if(countTimer==5000)countTimer=0;
	
	FLAG_PID=1; //lo llamo cada 1ms despues lo subo
	
	/*if(timerInt!=0){// si es 0 es que la velociadad es 0, y lo que hago es no actualizar nada
		//si el tiempo puesto es menor al tiempo minimo
		if(timerInt<TIEMPO_MINIMO_ENTRE_PASO) timerInt=TIEMPO_MINIMO_ENTRE_PASO;
		//if(timerInt<4) timerInt=4;
		
		if(++countTimer>=timerInt){	//lo que tengo que hacer es multiplicar timerInt cuadno lo calculo para no perder exactutud
			if( (pos+lado>inicio) && (pos+lado<FIN) ){	
				//si me pase de los ms que tenia que contar hago un paso
				pos+=lado;
				ESCRIBIRMOTOR(movLower[pos%8]);
				countTimer=0;
			}
		}
		
	}else{
		//esto lo hago para que cuando la velocidad es 0, y despues se tenga que mover primero da un paso
		countTimer=100;
	}*/
}




void buscarOrigen(){
	cli();
	//configurar el puerto de entrada pin6 D6
	DDRD &= ~(1<<PORTD6);
	//colocar el pin con pull up
	PORTD |= (1<<PORTD6);
	
	//prendo el led indicando que estoy buscando el origen
	PORTD |= (1<<PORTD7);
	
	
	//me muevo rapido buscando el origen
	pos=FIN;
	while((PIND & (1<<PORTD6))){	//mientras no toque el boton
		ESCRIBIRMOTOR(movLower[pos%8]);
		pos--;
		_delay_ms(3);
	}
	
	
	PORTD &= ~(1<<PORTD7);
	
	//me vuelvo un poquito
	moverPasos(80);
	_delay_ms(500);
	
	
	//vuelvo a buscar el origen mas despacio
	pos=FIN;
	while((PIND & (1<<PORTD6))){	//mientras no toque el boton
		ESCRIBIRMOTOR(movLower[pos%8]);
		pos--;
		_delay_ms(7);
	}
	
	//lo pongo en un limite inferior
	pos=pos%8;
	inicio=pos;	
	
	//apago el led que encontre el origen
	PORTD &= ~(1<<PORTD7);
	sei();
}


