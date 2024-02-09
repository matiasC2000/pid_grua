/*
 * PID.h
 *
 * Created: 8/16/2023 2:26:32 PM
 *  Author: Calvo
 */ 


#ifndef PID_H_
#define PID_H_

void Actulizar_PID();
int16_t getError();
int16_t getVelocidadPID();
int16_t getDerivadaPID();
int16_t getIntegralPID();
int16_t getTiempoDev();
//void setCero(int16_t angulo);

#endif /* PID_H_ */