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

int16_t leerAngulo();
int16_t leerVelocidad();
uint8_t hayDato();
void mostrarDatos();

#endif /* SENSORENCODER_H_ */