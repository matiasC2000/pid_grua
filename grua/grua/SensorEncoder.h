/*
 * SensorEncoder.h
 *
 * Created: 8/1/2023 3:49:20 AM
 *  Author: Calvo
 */ 


#ifndef SENSORENCODER_H_
#define SENSORENCODER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"


void SensorEncoder_init();

int16_t leerAnguloEncoder();
uint8_t hayDato();
void mostrarDatosEncoder();
uint32_t SensorEncoder_getTiempo();

#endif /* SENSORENCODER_H_ */