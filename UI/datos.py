
#esta clase es una singleton que se ccomunica siempre con la UI

import os
import pandas as pd

from matplotlib import pyplot as plt
import numpy as np

from variablesGlobales import *
from abc import ABC, abstractmethod
import threading
import serial as sr

def hex_to_int(hex_str):
    """
    Convierte un número hexadecimal en complemento a 2 a un entero.
    Args:
        hex_str (str): El número hexadecimal en complemento a 2.
    Returns:
        int: El valor entero correspondiente.
    """
    num_bytes = bytes.fromhex(hex_str)  # Convertir el hexadecimal a bytes
    int_value = int.from_bytes(num_bytes, byteorder='big', signed=True)  # Convertir los bytes a entero
    return int_value

class GeneradorDatos(ABC):
    def __init__(self):
        self.array = []
        self.estado_conexion = None
        self.guardar = False
        self.detener = False
        self.file_path = ""
        self.file_path_ref = ""
        self.datoNuevoEnviar = False

        self.hilo = None

        self.t_values = [0]
        self.set_values = [0]
        self.d_values = [0]
        self.i_values = [0]
        self.e_values = [0]

    @abstractmethod
    def inicio(self,*args):
        pass

    def stop(self):
        self.detener = True
        self.guardar = False

    @abstractmethod
    def leer_datos(self):
        pass


    def get_datos(self):
        if self.datoNuevoEnviar:
            self.datoNuevoEnviar=False
            return [self.t_values, self.e_values, self.set_values]
        else:
            return None

    def guardar_archivo(self):
        with open(self.file_path, 'a') as file:
            data = f"{self.t_values[-1]};{self.e_values[-1]};{self.d_values[-1]};{self.i_values[-1]};{self.set_values[-1]}"
            file.write('\n' + data)

    @abstractmethod
    def cambiar_coef_PID(self):
        pass

    def obtener_array(self):
        return self.array

    def start_hilo(self):
        print("Hilo", "iniciado")
        while not self.detener:
            self.leer_datos()
            self.actualizar_datos()
            if self.guardar:
                self.guardar_archivo()

class GeneradorArduino(GeneradorDatos):
    def __init__(self):
        super().__init__()
        self.estado_conexion = False
        self.ecuacion = 0
        self.s = None
        self.file_path_ref = "arduino"
        self.kd = 0

    def inicio(self,*args):
        print("entro a inicio Arduino")
        print(args[0][0])
        if self.estado_conexion == False:
            #si ya se conecto no creo otro hilo
            try:
                self.s = sr.Serial(args[0][0], 250000)                        #tengo que ver como le paso el parametro
                #self.s.timeout = 5  # Establece un tiempo de espera de 5 segundos
                self.hilo = threading.Thread(target=self.start_hilo)
                self.hilo.start()
                self.estado_conexion = True
            except Exception as e:
                self.estado_conexion = False
                print('error de coneccion')

    def leer_datos(self):
        a = ""
        try:
            a = self.s.readline()
        except Exception as e:
            print("no se pudo leer, reconecto")
            self.estado_conexion = False
            self.detener = True
        try:
            a = a.decode()
        except Exception as e:
            print('fallo')
            a=""
        self.a = a


    def actualizar_datos(self):
        if self.a != "":
            self.a = self.a.replace('\n',';')
        data_values = self.a.split(';')
        if len(data_values) == 6: #lleva un mas por uno que se agrega al final
            try:
                
                axit = hex_to_int(data_values[0][0:])/1000
                axie = hex_to_int(data_values[1][0:])
                axii = hex_to_int(data_values[2][0:])
                axid = hex_to_int(data_values[3][0:])
                axiset = hex_to_int(data_values[4][0:])
                # Intenta convertir la cadena a un número entero en hexadecimal
                self.t_values.append(axit)
                self.e_values.append(axie)
                self.i_values.append(axii)
                self.d_values.append(axid)
                if self.ecuacion != 2:
                    self.set_values.append(0)
                else:
                    self.set_values.append(axiset)
                #print(t_values[-1],e_values[-1],set_values[-1])
                self.datoNuevoEnviar = True
            
            except ValueError:
                print("Arduino: "+self.a)
            # Si no se puede convertir a un número en hexadecimal, devuelve False
            
        while(self.t_values[-1] - self.t_values[0] > tiempoEnArreglo):
            del self.t_values[0]
            del self.e_values[0]
            del self.set_values[0]

    def cambiar_coef_PID(self,kp,ki,kd,ecuacion):
        # Convertir los valores a bytes
        self.ecuacion = ecuacion
        self.kd = kd
        mensaje = f"{kp} {ki} {kd} {ecuacion}\r\n".encode()  # Convertir a una cadena y luego a bytes

        # Enviar el mensaje a través del puerto serie
        self.s.write(mensaje)
        print('Mensaje enviado:', mensaje)

    def get_datos(self):
        return [self.t_values,self.e_values,self.set_values]

class GeneradorSimulacion(GeneradorDatos):
    def __init__(self):
        super().__init__()
        print("se creo simulacion")
        self.estado_conexion = True
        self.file_path_ref = "simulacion"
        self.kp = 0
        self.ki = 0
        self.kd = 0

    def inicio(self, *args):
        #Genero la inferface que quiero
        print("inicio el hilo de simulacion")
        self.estado_conexion = False
        self.simulador = Simulacion(int(args[0][0]),0,self.kp,self.ki,self.kd)
        self.hilo = threading.Thread(target=self.mira_el_otro_hilo)
        self.hilo.start()

    def mira_el_otro_hilo(self):
        while(not self.simulador.fin):
            pass
        dato = InterfaceDatos()
        dato.inicio_guardar_datos()
        datos = self.simulador.get_datos()
        df = pd.DataFrame({
            't': datos[0],
            'e': datos[1],
            'i': datos[2],
            'd': datos[3],
            'set': datos[4]
        })

        # Guardar el DataFrame en un archivo separado por punto y coma
        df.to_csv(self.file_path, sep=';', index=False)
        self.estado_conexion = True

    def leer_datos(self):
        t, e, i, d, set_ = self.simulador.get_ultimo()
        self.es_nuevo = t != self.t_values[-1]
        if self.es_nuevo:
            self.t_values.append(t)
            self.e_values.append(e)
            self.i_values.append(i)
            self.d_values.append(d)
            self.set_values.append(set_)
            self.datoNuevoEnviar = False

    def actualizar_datos(self):
        #la tengo que dejar solo 2 seg en el arreglo
        while(self.t_values[-1] - self.t_values[0] > tiempoEnArreglo):
            del self.t_values[0]
            del self.e_values[0]
            del self.set_values[0]


    def guardar_archivo(self):
        if self.es_nuevo:
            with open(self.file_path, 'a') as file:
                data = f"{self.t_values[-1]};{self.e_values[-1]};{self.d_values[-1]};{self.i_values[-1]};{self.set_values[-1]}"
                file.write('\n' + data)
        
        #me quedo esperando a que termine el tiempo que tiene que pasar

    def cambiar_coef_PID(self,kp,ki,kd,ecuacion):
        self.kp = float(kp)
        self.ki = float(ki)
        self.kd = float(kd)
        print("mande datos")


class InterfaceDatos:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super().__new__(cls, *args, **kwargs)
            # Si aún no se ha creado el generador de datos, créalo aquí
            cls.generadordatos = GeneradorArduino()
            cls.ruta = ""
            cls.guardarDatos = False
            cls.stateSimulacion = False
        return cls._instance
 
    def inicio(self, *args):
        #esto lo llama el boton conecct o el lanzar
        self.generadordatos.inicio(args)
        print("interface Datos llama a inicio")

    def detener(self):
        self.generadordatos.stop()
        print("espero a que termine el hilo")
        if self.generadordatos.hilo != None:
            print("estaba")
            #self.generadordatos.hilo.join()
        print("rip")

    def actualizar_coef_PID(self, kp, ki ,kd,ecuacion):
        print("simulacion", self.stateSimulacion)
        self.generadordatos.cambiar_coef_PID(kp,ki ,kd,ecuacion)
        pass

    def cambiar_arduino(self):
        if self.generadordatos != None:
            self.generadordatos.stop()
        self.generadordatos = GeneradorArduino()
        
    def cambiar_simulacion(self):
        if self.generadordatos != None:
            self.generadordatos.stop()
        self.generadordatos = GeneradorSimulacion()

    def inicio_guardar_datos(self):
        #Chequear que no este guardando
        #crear el archivo
        if not self.guardarDatos:
            print("inicio_guardar_datos")
            output_folder = "datos"  # Carpeta para almacenar los archivos TXT
            if not os.path.exists(output_folder):
                os.makedirs(output_folder)
            #cambiar la ruta
            file_number = 1
            while True:
                filename = f'datos/datos_{self.generadordatos.file_path_ref}_{file_number}.txt'
                if not os.path.exists(filename):
                    break
                file_number += 1
                # Obtener la ruta absoluta del archivo
            
            absolute_path = os.path.abspath(filename)
            # Asignar la ruta absoluta al generador de datos
            self.generadordatos.file_path = absolute_path
            print("dir"+self.generadordatos.file_path)

            with open(self.generadordatos.file_path, 'a') as file:
                file.write("t;e;d;i;set"+'\n')            

            #habilitar el guardado
            self.generadordatos.guardar = True
            self.guardarDatos = True
            
    def detener_guardar_datos(self):
        print("detenet guarda")
        #deshabilitar el guardado
        self.generadordatos.guardar = False
        self.guardarDatos = False

    def get_estado(self):
        return self.generadordatos.estado_conexion
    
    def get_datos(self):
        return self.generadordatos.get_datos()
    
    def seleccionar_ruta(self, ruta):
        self.ruta = ruta
        print("selecionar archivo"+self.ruta)

    def abrir_archivo(self):
        #crear el grafico a partir de los datos
        print("ruta: "+self.ruta)
        with open(self.ruta, 'r') as archivo:
            lineas = archivo.readlines()

        t_values = []
        e_values = []
        d_values = []
        i_values = []
        set_values = []

        data = pd.read_csv(self.ruta, delimiter=';')

        t_values = data['t'].tolist()  # 't_column' es el nombre de la columna que contiene los valores de t
        e_values = data['e'].tolist()  # 'e_column' es el nombre de la columna que contiene los valores de e
        d_values = data['d'].tolist()  # 'd_column' es el nombre de la columna que contiene los valores de d
        i_values = data['i'].tolist()  # 'i_column' es el nombre de la columna que contiene los valores de i
        set_values = data['set'].tolist()  # 'set_column' es el nombre de la columna que contiene los valores de set

        fig, axs = plt.subplots(1, 1, figsize=(10, 8))
        #t_values = [x/1000 for x in t_values]

        axs.plot(t_values, e_values,'.-')
        axs.plot(t_values, set_values)
        axs.set_xlabel('Time')
        axs.set_ylabel('Pos')
        axs.set_title('Pos vs Time')
        axs.grid(True)

        fig2, axs2 = plt.subplots(1, 1, figsize=(10, 8))

        error = []
        for i in range(len(e_values)):
            error.append(e_values[i]-set_values[i])

        axs2.plot(t_values, error,'r.-',label='error')
        axs2.plot(t_values, d_values,'b',label='derivada')
        axs2.plot(t_values, i_values,'g',label='integral')
        axs2.set_xlabel('Time')
        axs2.set_ylabel('Pos')
        axs2.set_title('Pos vs Time')
        axs2.grid(True)

        plt.legend(loc="upper left")
        fig.tight_layout()
        plt.show()

from math import tau
import time


class Simulacion:
    def __init__(self, theta, theta_dot, kp,ki,kd):
        self.fin = False
        self.igual = 0
        self.detener = False

        # Define las variables como atributos de instancia
        self.kappa = 186.25294289
        self.mu = 0.48631962
        self.gamma = 0.5766281
        self.Lambda = self.kappa/9.8

        self.dt = 0.1*1/1000
        self.dt_system = 30
        self.full_step_time = 40*self.dt
        self.step_size = 0.4*1/1000
        self.car_reaction_time = 5*10
        self.sensor_reaction_time = 3*10
        #la derivada se calcula cada 30ms nose cual es la unidad aca
        self.derivative_calculation_time = 11*10
        self.res = 1
        self.Max_int = 0.5
        self.Max_x = .2

        self.tiempoAnt=[0,0]
        self.posAnt=[0,0]

        # inicializa variables simuladas
        self.i = 0
        self.theta_int = 0
        self.theta_mesured = theta
        self.theta_dot_mesured = 0
        self.move = False
        self.waiting_time = 0
        self.direc = None
        self.wanted_direc = None
        self.step_time = 0
        self.theta = theta
        self.theta_dot = theta_dot
        self.Dtheta_dot = None
        self.x = 0
        self.v = 0
        self.a = None 
        self.error = None
        
        self.kp = kp
        self.kd = kd
        self.ki = ki

        self.t = [0,0]
        self.int = [0,0]
        self.e = [0,0]
        self.d = [0,0]
        self.set_ = [0,0]


        self.hilo = threading.Thread(target=self.nuevo_paso)
        self.hilo.start()

    def cap(self, val, Max): # capping a value to a maximum to the positives and negatives
        return min(abs(val), Max) * np.sign(val)

    def array_cap(self, val, Max): # =cap, used for ndarrays
        return np.where(abs(val) < Max, val, Max)[0] * np.sign(val)
    
    def calcularSen(self, sen):
        sen2 = [0,7,15,23,31,39,47,54,62,70,78,86,94,101,109,117,125,133,140,148,156,164,171,179,187,195,202,210,218,225,233,241,248,256,263,271,278,286,294,301,309,316,323,331,338,346,353,360,368,375,382,389,397,404,411,418,425,432,439,446,453,460,467,474,481,488,495,502,509,515,522,529,535,542,549,555,562,568,575,581,587,594,600,606,612,619,625,631,637,643,649,655,661,667,673,678,684,690,695,701,707,712,718,723,728,734,739,744,750,755,760,765,770,775,780,785,790,794,799,804,809,813,818,822,827,831,835,840,844,848,852,856,860,864,868,872,876,880,883,887,891,894,898,901,904,908,911,914,917,920,923,926,929,932,935,938,940,943,946,948,951,953,955,958,960,962,964,966,968,970,972,974,975,977,979,980,982,983,985,986,987,988,990,991,992,993,993,994,995,996,996,997,998,998,998,999,999,999,999,999,1000]
        sen = int(sen)
        sen= sen%800
        if sen<0:
            sen = 800+sen
        if sen>399:
            sen = sen%400
            if sen>200: sen = 200- sen%200
            sen = - sen2[sen]
        else:
            sen = sen%400
            if sen>200: sen = 200-(sen%200)
            sen = sen2[sen]
        return sen
    
    
    def derivative_calculation_PID(self,theta):
        e = theta
        derivative = 0
        if e == self.posAnt[0]:
            derivative = e-self.posAnt[1]
            tiempoDev = self.i-self.tiempoAnt[1]
            derivative = derivative#/tiempoDev	
        if e != self.posAnt[0] :
            derivative = e-self.posAnt[0]
            tiempoDev = self.i-self.tiempoAnt[0]
            derivative = derivative
			
            self.posAnt[1]=self.posAnt[0]
            self.tiempoAnt[1]=self.tiempoAnt[0]
            self.posAnt[0]=e
            self.tiempoAnt[0]=self.i
        return derivative

    def PID(self, theta_int,theta,theta_dot):#PID system for controling the angle, returns a "velocity" which is used to calculate the time the car needs to wait until the next move
        self.errorPID = theta
        #self.errorPID = self.resolution(self.shift(theta,400),self.res)
        #self.errorPID = self.calcularSen(self.errorPID)
        #theta_dot = self.derivative_calculation_PID(self.errorPID)
        response_vel = (
            self.kp*self.errorPID+
            self.kd*self.theta_dot +
            self.ki*self.theta_int
            )
        response_vel = self.cap(response_vel, self.step_size/(self.car_reaction_time*self.dt))
        return -response_vel

    def resolution(self, val, Min): # return the value unless its under the resolution of the sensor in which case returns zero
        return np.where(np.abs(val) > Min, val, 0)

    def Dtheta_dot_func(self, theta, theta_dot, a): # calculate the angle acceleration according to the model, taking the car's motion into account
        Dtheta_dot = -self.Lambda * a * np.cos(theta * tau / 800) - self.kappa * np.sin(theta * tau / 800) - self.mu * theta_dot * tau / 800
        Dtheta_dot = np.where((np.abs(Dtheta_dot) < np.abs(self.gamma)) & (theta_dot == 0), 0, Dtheta_dot - self.gamma * np.sign(theta_dot))
        Dtheta_dot /= tau / 800
        return Dtheta_dot

    def sensor_actualize(self, theta): # get the information of the angle save it as a variable used for control and add to the integral
        if not self.i % self.derivative_calculation_time:
            self.theta_dot_mesured = np.round(theta) - self.theta_mesured
        self.theta_mesured = np.round(theta)
        error = self.resolution(self.shift(self.theta_mesured, 400), self.res)
        self.theta_int += error * self.sensor_reaction_time * self.dt
        self.theta_int = self.cap(self.theta_int, self.Max_int)

    def shift(self, angle, shift, tau=800): # cap the angle's possible values to -400 to +400 so the PID finds the closest way to get to the needed angle
        return (angle - shift + tau / 2) % tau - tau / 2
    
    def step_func(self, t):
        # Compute position, velocity, and acceleration functions
        x = -np.sin(tau * t / self.full_step_time) + tau * t / self.full_step_time
        v = -tau / self.full_step_time * np.cos(tau * t / self.full_step_time) + tau / self.full_step_time
        a = (tau / self.full_step_time) ** 2 * np.sin(tau * t / self.full_step_time)
        # Scale by step_size/tau
        x *= self.step_size / tau
        v *= self.step_size / tau
        a *= self.step_size / tau
        return x, v, a
                
    
    def car_actualize(self, new_waiting_time):
        if self.waiting_time <= 0:
            self.move = True
            self.direc = self.wanted_direc
            self.waiting_time = new_waiting_time
        else:
            self.waiting_time = min(self.waiting_time - self.dt_system, new_waiting_time)

    def car_physics(self):
        if self.move:
            delta_x, v, a = self.step_func(self.step_time)
            self.step_time += self.dt
            if self.step_time > self.full_step_time:
                self.move = False
                self.step_time = 0
            return self.direc * np.array((delta_x, v, a))
        else:
            return np.zeros(3)  # Return zero if the car isn't moving
    
    def physics(self):
        if self.move:
            delta_x, self.v, self.a = self.car_physics()
            self.x += self.v * self.dt
        else:
            self.x, self.v, self.a = self.x, 0, 0
        if abs(self.x) > self.Max_x:
            if self.x>0: self.x = self.Max_x
            else: self.x = - self.Max_x
        self.Dtheta_dot = self.Dtheta_dot_func(self.theta, self.theta_dot, self.a)
        self.theta += self.theta_dot * self.dt
        self.theta_dot += self.Dtheta_dot * self.dt
    
    def control(self):
        if not self.i % self.sensor_reaction_time:
            self.sensor_actualize(self.theta)
        
        response_vel = self.PID(self.theta_int, self.theta_mesured, self.theta_dot_mesured)    
        self.wanted_direc = np.sign(response_vel)
        
        if response_vel != 0:
            new_waiting_time = abs(self.step_size / response_vel)
        else:
            new_waiting_time = 10000000
        
        self.car_actualize(new_waiting_time)

    def actualizar_datos_salida(self):
        self.t.append(self.i*self.dt)
        self.int.append(self.theta_int)
        self.e.append(self.errorPID)
        self.d.append(self.theta_dot_mesured)
        self.set_.append(0)      

    def nuevo_paso(self):
        while self.i < 200000 and not self.detener: #0.1*1/1000
            inicio = time.time()
            self.physics()
            if not self.i % self.dt_system:
                self.control()
                self.actualizar_datos_salida()
                if self.e[-1] == self.e[-2]:
                    self.igual = self.igual + 1
                    if self.igual == 100:
                        self.detener = True
                else:
                    self.igual = 0
            
            #if abs(self.x) > self.Max_x:
                #print('car fell')
                #raise Exception 
            #total += time.time() - inicio
            self.i += 1
        self.fin = True

    def get_datos(self):
        return (self.t, self.e, self.int ,self.d, self.set_)

    def get_ultimo(self):
        return (self.t[-1], self.e[-1], self.int[-1], self.d[-1], self.set_[-1])


if __name__ == "__main__":
    #datos = InterfaceDatos()
    #datos.cambiar_simulacion()
    #datos.inicio(300)

    
    simu = Simulacion(300,0)
    inicio = time.time()
    simu.nuevo_paso()
    fin = time.time()
    timpoTardo = fin-inicio
    print("2 seg lo hizo en: %f",timpoTardo)

