/*
 * UART.c
 *
 * Created: 10/19/2023 1:30:07 PM
 *  Author: Calvo
 */ 


#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define UART_BUFFER_SIZE 230  // Tamaño del buffer UART circular
#define UART_BUFFER_SIZE_RX 100

static char tx_buffer[UART_BUFFER_SIZE];
static uint8_t tx_buffer_read = 0;
static uint8_t tx_buffer_write = 0;

static char rx_buffer[UART_BUFFER_SIZE_RX];
static uint8_t rx_buffer_read = 0;
static uint8_t rx_buffer_write = 0;

void UART_Init(){
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
	UBRR0L = 8; // baud rate = 115200bps (para F_CPU = 16MHz)
	
	UCSR0B |= (1 << RXEN0);  // habilito la RX
	UCSR0B |= (1 << RXCIE0); // habilito la interrupcion
	UCSR0B |= (1 << TXEN0);  // habilito la TX
	UCSR0B |= (1 << UDRIE0); // habilito la interrupcion de cuando esta vacio
	UCSR0A |= (1 << UDRE0);  // Habilita el flag de cuando se pueden dejar datos
}


uint8_t getEspacioDisponibleBuffer() {
	uint8_t availableSpace;
	// Calcula el espacio disponible en el buffer
 	if (tx_buffer_write >= tx_buffer_read) {
 		availableSpace = UART_BUFFER_SIZE - tx_buffer_write + tx_buffer_read;
 	} else {
 		availableSpace = tx_buffer_read - tx_buffer_write;
 	}
	return availableSpace;
}



void UART_TransmitChar(char data) {
	// Agregar el carácter al buffer circular de transmisión
	uint8_t next_write = (tx_buffer_write + 1) % UART_BUFFER_SIZE;
	if (next_write != tx_buffer_read) {
		tx_buffer[tx_buffer_write] = data;
		tx_buffer_write = next_write;
		UCSR0B |= (1 << UDRE0); // Habilita la int cuando le dejo datos
	}
}


void UART_TransmitString(const char* data,uint8_t size) {
	if(size<getEspacioDisponibleBuffer()){
		while (*data) {
			UART_TransmitChar(*data);
			data++;
		}
	}
}


ISR(USART_UDRE_vect) {
	// Transmitir datos desde el buffer circular de transmisión
	if (tx_buffer_read != tx_buffer_write) {
		UDR0 = tx_buffer[tx_buffer_read];
		tx_buffer_read = (tx_buffer_read + 1) % UART_BUFFER_SIZE;
	}else{
		UCSR0B &= ~(1 << UDRE0); // Deshabilita la int cuando no hay datos en el buffer
	}
}


void SacarDatosBuffer(uint8_t *datos){
	//cli(); // Deshabilitar interrupciones mientras copiamos el buffer
	uint8_t i = 0;
	while (rx_buffer_write != rx_buffer_read && i < UART_BUFFER_SIZE_RX - 1) {
		datos[i] = rx_buffer[rx_buffer_read];
		rx_buffer_read = (rx_buffer_read + 1) % UART_BUFFER_SIZE_RX;
		i++;
	}
	//sei(); // Habilitar interrupciones nuevamente					de ultima saco los qe entren
	datos[i] = '\0';
}

ISR(USART_RX_vect){
	uint8_t newRxHead = (rx_buffer_write + 1) % UART_BUFFER_SIZE_RX;
	uint8_t receivedChar = UDR0;
	
	if (newRxHead != rx_buffer_read) {
		rx_buffer[rx_buffer_write] = receivedChar;
		rx_buffer_write = newRxHead;
	}
}