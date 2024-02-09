/*
 * sensor.h
 *
 * Created: 7/17/2023 11:55:10 AM
 *  Author: Calvo
 */ 


#ifndef SENSOR_H_
#define SENSOR_H_
#include <avr/io.h>
#include <avr/interrupt.h>


uint16_t ord;
uint16_t ords[3];

int16_t leerDato();
int16_t leerDatoInt();
void Inicializacion_Sensor();
void Inicializacion_Sensor_Int();
uint16_t dameOrdenada();

#endif /* SENSOR_H_ */