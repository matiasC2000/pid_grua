/*
 * PID.c
 *
 * Created: 8/2/2023 6:24:35 PM
 *  Author: Calvo
 */ 

#include <avr/io.h>

int16_t eant = 0;
int16_t tant = 0;
int16_t s = 0;
int16_t Kp = 3;
int16_t Ki = 1;
int16_t dt;
int16_t diff;
int16_t vel;
int16_t altura;

int PID(int8_t e,uint16_t t){
	dt = t-tant;
	tant = t;
	diff = eant - e;
	altura = e + diff/2;
	s += altura*dt;
	eant = e;
	vel = Kp*e + Ki*s;
	return vel;
}

