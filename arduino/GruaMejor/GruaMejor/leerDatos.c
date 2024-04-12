/*
 * leerDatos.c
 *
 * Created: 3/12/2024 11:13:10 AM
 *  Author: Calvo
 */ 

#include "UART.h"
#include "avr/io.h"
#include "global.h"
#include "PID.h"

void actulizar_leerDatos(){
	char datosEntrada[50];
	if(SacarDatosBufferEnter(datosEntrada)){
		uint8_t i=0,k=0;
		float valores[]={0,0,0,0};
		float cantDigitos=1;
		UART_TransmitString("datos: ",10);
		UART_TransmitString(datosEntrada,10);
		while(datosEntrada[i]!='\0'){
			cantDigitos=1;
			while(datosEntrada[i]!=',' && datosEntrada[i]!='.' && datosEntrada[i]!=' ' && datosEntrada[i]!='\0'){
					if(datosEntrada[i]=='-'){
						cantDigitos = cantDigitos*-1;
						}else{
						valores[k] = (datosEntrada[i]-48)+valores[k]*10;
					}
					UART_TransmitChar(datosEntrada[i]);
					i++;
			}
			while(datosEntrada[i]!=' ' && datosEntrada[i]!='\0'){
				if(datosEntrada[i]!='.' && datosEntrada[i]!=','){
					valores[k] = (datosEntrada[i]-48)+valores[k]*10;
					cantDigitos=cantDigitos*10;
					
				}
				i++;
			}
			valores[k]=(valores[k]/cantDigitos);
			k++;
			if(datosEntrada[i]==' '){UART_TransmitChar(datosEntrada[i]); i++;}
		}
		if(valores[0]==-12.2){
			ONLEDBLUE;
		}else{
			OFFLEDBLUE;
		}
		if(k==4){
			setKp(valores[0]);
			setKi(valores[1]);
			setKd(valores[2]);
			setIndice_ec_error((uint8_t)valores[3]);
			UART_TransmitChar((uint8_t)valores[3]+48);
		}
	}
}