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
#define F_CPU 16000000UL
#include <util/delay.h>


uint16_t getPos();
void setVelocidad();
void Inicializacion_Motor();
void movimientoSexy(int16_t);
void irPos(uint16_t);
void irCentro();
void buscarOrigen();


#endif /* MOTOR_H_ */