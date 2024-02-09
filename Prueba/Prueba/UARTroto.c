/*
 * UART.c
 *
 * Created: 6/11/2023 7:36:18 PM
 *  Author: Calvo
 */ 
#include "UART.h"

uint8_t Index_RX = 0;
uint8_t FLAG_datos_recibidos = 0;

uint8_t Index_TX = 0;
uint8_t Index_TX_E = 0;


void Inicializacion_UART(){
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);//8N1
	//UBRR0L = 51; // baud rate = 19200bps (para F_CPU = 16MHz)
	UBRR0L = 103; //baud rate = 9600bps
	
	UCSR0B |= (1<<RXEN0);  //habilito la RX
	UCSR0B |= (1<<RXCIE0); //habilito la interrupcion
}

void AgregarDatosBuffer(uint8_t *datos){		
	uint8_t p = 0;
	//se copia el dato en el buffer
	while(datos[p]!='\0'){
		BufferTX[++Index_TX_E]=datos[p];
		p++;
	}
	BufferTX[++Index_TX_E] = '\0';
	UCSR0B |= (1<<TXEN0); //esto cada vez que quiero mandar un dato
	UCSR0B |= (1<<UDRE0); //Habilito la la int cuando le dejo datos la tengo que activar. No la dejo activa

	
}

void AgregarDatoBuffer(uint8_t dato){
	BufferTX[++Index_TX_E]=dato;
	if(dato == '\0'){
		UCSR0B |= (1<<TXEN0); //esto cada vez que quiero mandar un dato
		UCSR0B |= (1<<UDRE0); //Habilito la la int cuando le dejo datos la tengo que activar. No la dejo activa
	}
}

//para sacar los datos los leo directo del buffer para no perder tanto tiempo
//y supongo que el usuario no va a mandar datos tan rapido como para que se pisen

ISR(USART_UDRE_vect){//para cuando la UART esta libre
	UDR0 = BufferTX[Index_TX++];
	if(BufferTX[Index_TX] == '\0'){
		if (Index_TX == Index_TX_E)
		{
			Index_TX=0;
			Index_TX_E=0;
			UCSR0B &=~(1<<UDRE0);  //deshabilito la int cuando le dejo datos la tengo que activar. No la dejo activa
		}						   // porque intrrumpe todo el tiempo
	}
	
}

void SacarDatosBuffer(uint8_t *datos){
	//hacer copia del vector
	strcpy(datos, BufferRX);
	FLAG_datos_recibidos = 0;
}


ISR(USART_RX_vect){
	BufferRX[Index_RX++] = UDR0;
	if((BufferRX[Index_RX-1] == '\r')||(BufferRX[Index_RX-1]=='\n')){//BufferRX[Index_RX++] == '\n'
			BufferRX[Index_RX-1] = '\0';
			Index_RX = 0;
			FLAG_datos_recibidos= 1;
		}
}