/*
 * uart.h
 *
 * Created: 6/11/2023 7:36:18 PM
 *  Author: Calvo
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define BUFFER_SIZE_RX 255
#define BUFFER_SIZE_TX 255

extern uint8_t BufferRX[BUFFER_SIZE_RX];
extern volatile uint8_t RxHead;
extern volatile uint8_t RxTail;

extern uint8_t BufferTX[BUFFER_SIZE_TX];
extern volatile uint8_t TxHead;
extern volatile uint8_t TxTail;

void Inicializacion_UART();
void AgregarDatoBuffer(uint8_t dato);
void AgregarDatosBuffer(const uint8_t *);
void SacarDatosBuffer(uint8_t *datos);
uint16_t EspacioDisponibleBuffer();

#endif // UART_H
