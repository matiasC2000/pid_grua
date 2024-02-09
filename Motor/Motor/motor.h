/*
 * motor.h
 *
 * Created: 7/17/2023 5:16:06 PM
 *  Author: Calvo
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_


#include <avr/io.h>
#include <avr/interrupt.h>

void setVelocidad(int16_t );
void Inicializacion_Motor();

void irPos(uint16_t);
void buscarOrigen();
void movimientoSexy();
uint16_t getPos();




#endif /* MOTOR_H_ */