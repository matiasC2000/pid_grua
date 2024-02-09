/*
 * UART.c
 *
 * Created: 6/11/2023 7:36:18 PM
 *  Author: Calvo
 */ 
#include "UART.h"


uint8_t BufferRX[BUFFER_SIZE_RX];
volatile uint8_t RxHead = 0;
volatile uint8_t RxTail = 0;

uint8_t BufferTX[BUFFER_SIZE_TX];
volatile uint8_t TxHead = 0;
volatile uint8_t TxTail = 0;

void Inicializacion_UART(){
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
	UBRR0L = 51; // baud rate = 19200bps (para F_CPU = 16MHz)
	
	UCSR0B |= (1 << RXEN0);  // habilito la RX
	UCSR0B |= (1 << RXCIE0); // habilito la interrupcion
	UCSR0B |= (1 << TXEN0);  // habilito la TX
}

void AgregarDatosBuffer(const uint8_t *datos) {
	uint8_t p = 0;
	
	while (datos[p] != '\0') {
		AgregarDatoBuffer(datos[p]);
		p++;
	}
}

uint16_t EspacioDisponibleBuffer() {
	uint16_t availableSpace;

	// Calcula el espacio disponible en el buffer
	if (TxHead >= TxTail) {
		availableSpace = BUFFER_SIZE_TX - (TxHead - TxTail) - 1;
		} else {
		availableSpace = TxTail - TxHead - 1;
	}

	return availableSpace;
}



void AgregarDatoBuffer(uint8_t dato) {
	uint8_t newTxHead = (TxHead + 1) % BUFFER_SIZE_TX;
	
	if (newTxHead != TxTail) {
		BufferTX[TxHead] = dato;
		TxHead = newTxHead;
		UCSR0B |= (1 << UDRE0); // Habilita la int cuando le dejo datos
		} else {
		// Buffer lleno, ignora el nuevo dato o maneja el desbordamiento si es necesario
	}
}


ISR(USART_UDRE_vect){
	if (TxHead != TxTail) {
		UDR0 = BufferTX[TxTail];
		TxTail = (TxTail + 1) % BUFFER_SIZE_TX;
	} else {
		UCSR0B &= ~(1 << UDRE0); // Deshabilita la int cuando no hay datos en el buffer
	}
}

void SacarDatosBuffer(uint8_t *datos){
	cli(); // Deshabilitar interrupciones mientras copiamos el buffer
	uint8_t i = 0;
	while (RxHead != RxTail && i < BUFFER_SIZE_RX - 1) {
		datos[i] = BufferRX[RxTail];
		RxTail = (RxTail + 1) % BUFFER_SIZE_RX;
		i++;
	}
	sei(); // Habilitar interrupciones nuevamente
	datos[i] = '\0';
}

ISR(USART_RX_vect){
	uint8_t newRxHead = (RxHead + 1) % BUFFER_SIZE_RX;
	uint8_t receivedChar = UDR0;
	
	if (newRxHead != RxTail) {
		BufferRX[RxHead] = receivedChar;
		RxHead = newRxHead;
	}
}



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
	uint32_t divisor = 1000000;
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