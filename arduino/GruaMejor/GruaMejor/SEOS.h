/*
 * SEOS.h
 *
 * Created: 5/14/2023 10:12:14 PM
 *  Author: Calvo
 */ 


#ifndef SEOS_H_
#define SEOS_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t tiempoSEOS;

//funciones publicas
void SEOS_SCH_Tasks(void);
void SEOS_Dispatch_Tasks(void);
void SEOS_Init(void);
uint32_t getTiempoSEOS();
uint32_t getTiempoInforme();

#endif /* SEOS_H_ */