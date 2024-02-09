/*
 * UART.h
 *
 * Created: 6/11/2023 9:01:48 PM
 *  Author: Calvo
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

uint8_t FLAG_datos_recibidos;
uint8_t BufferTX[150];
uint8_t BufferRX[150];

void Inicializacion_UART();
void AgregarDatosBuffer(uint8_t *datos);
void SacarDatosBuffer(uint8_t *datos);
void AgregarDatoBuffer(uint8_t);//Agregar de a un dato y pones /0 para mandar todo

#endif /* UART_H_ */