/*
 * Informar.c
 *
 * Created: 10/25/2023 12:22:53 PM
 *  Author: Calvo
 */

#include "UART.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "SensorEncoder.h"
#include "SEOS.h"
#include "PID.h"
#include "slideResistor.h"

void InformarPID();
void InformarSensor();
void customStrcat(char *dest, const char *src);
void Descomponeruint32(uint32_t numero,char *str);
void Descomponerint16(int16_t numero,char *str);
void Descomponeruint16(uint16_t numero,char *str);
uint8_t FLAGmandarDatos=0;

void Informar_MensajeInicial(){
	//UART_TransmitString("hola soy la grua\n",1);
	UART_TransmitString("tiempo;error;integral;derivada;setPoint\n",1);//tiempo;angulo;velocidad;derivada
}


//aca pongo los mensajes que quiero mandar
void Informar_Actulizar(){
	char tiempo[10],velocidad[7];
	char derivada[7],angulo[7];
	char integral[7];
	char error[7];
	int16_t valorAngulo;
	if(getEspacioDisponibleBuffer()>222){
		FLAGmandarDatos=1;
	}else{
		if(getEspacioDisponibleBuffer()<40){
			FLAGmandarDatos=0;
		}
	}
	if(FLAGmandarDatos){
		Descomponeruint32(getTiempoInforme(),tiempo);
		UART_TransmitString(tiempo,10);
		UART_TransmitChar(';');
		Descomponerint16(getValor(),error);
		UART_TransmitString(error,7);
// 		getAnguloEncoder(&valorAngulo,0);
// 		Descomponerint16(valorAngulo,angulo);
// 		UART_TransmitString(angulo,7);
		UART_TransmitChar(';');
		Descomponerint16(getIntegralPID(),integral); // Convierte a base decimal (10)
		UART_TransmitString(integral,7);
// 		Descomponerint16(getVelocidadPID(),velocidad); // Convierte a base decimal (10)
// 		UART_TransmitString(velocidad,7);
		UART_TransmitChar(';');
		Descomponerint16(getDerivadaPID(),derivada); // Convierte a base decimal (10)
		UART_TransmitString(derivada,7);
		UART_TransmitChar(';');
		Descomponerint16(getSlideResistor()*13.3,error);
		UART_TransmitString(error,7);
		UART_TransmitChar('\n');
	}
}

/*
	char numeroStr[7];
	pasar unit8
	utoa(num, numeroStr, 10); // Convierte a base decimal (10)
	pasar unit32
	utoa(num, numeroStr, 10); // Convierte a base decimal (10)
	int16
	itoa(num, numeroStr, 10); // Convierte a base decimal (10)
*/
void Descomponeruint32(uint32_t numero, char *str)
{
	/* Initial memory allocation */
	if(numero!=0){
		uint32_t div= 1000000000;
		while(numero/div == 0){
			div = div/10;
		}
		while(div>0){
			*(str)=48+numero/div;
			numero=numero%div;
			div = div/10;
			str++;
		}
	}else{
		*str=48;
		str++;
	}
	*(str) = '\0';
}

void Descomponeruint16(uint16_t numero,char *str){
	if(numero!=0){
		uint32_t div= 10000;
		while(numero/div == 0){
			div = div/10;
		}
		while(div>0){
			*(str)=48+numero/div;
			numero=numero%div;
			div = div/10;
			str++;
		}
		}else{
		*str=48;
		str++;
	}
	*(str) = '\0';
}

void Descomponerint16(int16_t numero, char *str)
{
	/* Initial memory allocation */
	if(numero<0) {
		numero = (-1)*numero;
		*str='-';
		str++;
	}
	if(numero!=0){
		int16_t div= 10000;
		while(numero/div == 0){
			div = div/10;
		}
		while(div>0){
			*(str)=48+numero/div;
			numero=numero%div;
			div = div/10;
			str++;
		}
	}else{
		*str=48;
		str++;
	}
	*(str) = '\0';
// 	uint16_t divisor = 10000;
// 	int8_t digito;
// 	int16_t resto = numero;
// 	
// 	while (divisor >= 1)
// 	{
// 		digito = resto / divisor;
// 		resto = resto % divisor;
// 		divisor = divisor / 10;
// 
// 		AgregarDatoBuffer(digito+48);
// 	}
}

// Prototipo de la función customStrcat
void customStrcat(char *dest, const char *src) {
	while (*dest != '\0') {
		dest++;
	}
	
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	
	*dest = '\0';  // Agrega el carácter nulo al final de la cadena resultante
}