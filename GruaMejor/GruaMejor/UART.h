#ifndef UART_H
#define UART_H

#include <stdint.h>

// Función para inicializar la UART
void UART_Init();

// Función para enviar un carácter a través de la UART
void UART_TransmitChar(char data);

// Función para enviar una cadena de caracteres a través de la UART
void UART_TransmitString(const char* data, uint8_t);

// Función para recibir un carácter de la UART (no bloqueante)
uint8_t UART_ReceiveChar(char* data);

// Función para recibir una cadena de caracteres de la UART (no bloqueante)
uint8_t UART_ReceiveString(char* buffer);

uint8_t getEspacioDisponibleBuffer();


#endif
