#ifndef UART_H
#define UART_H

#include <stdint.h>

// Funci�n para inicializar la UART
void UART_Init();

// Funci�n para enviar un car�cter a trav�s de la UART
void UART_TransmitChar(char data);

// Funci�n para enviar una cadena de caracteres a trav�s de la UART
void UART_TransmitString(const char* data, uint8_t);

// Funci�n para recibir un car�cter de la UART (no bloqueante)
uint8_t UART_ReceiveChar(char* data);

// Funci�n para recibir una cadena de caracteres de la UART (no bloqueante)
uint8_t UART_ReceiveString(char* buffer);

uint8_t getEspacioDisponibleBuffer();

uint8_t SacarDatosBufferEnter(uint8_t *datos);


#endif
