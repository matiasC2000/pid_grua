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



void SensorEncoder_init();

void getAnguloEncoder(int16_t *anguloOUT, uint32_t *tiempoMuestraOUT);

#endif /* SENSORENCODER_H_ */