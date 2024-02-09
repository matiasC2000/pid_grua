import os

import matplotlib.pyplot as plt
import pandas as pd
import serial
import matplotlib.animation as animation
import threading
import queue
import numpy as np
import math

def leer_datos_serie(ser, data_queue):
    while True:
        line = ser.readline().decode().strip()  # Lee una línea del puerto serie y la decodifica
        data_queue.put(line)

def guardar_datos_en_archivo(data_queue, file_path):
    while True:
        data = data_queue.get()
        if data.startswith("tiempo"):
            # Crear un archivo de texto enumerado
            with open(file_path, 'w') as file:
                file.write(data)
        else:
            if file_number > 0:
                with open(file_path, 'a') as file:
                    file.write('\n' + data)

def update_line(num,h1,data_queue):
    data = data_queue.get()
    data = "111;222;333"
    columns = data.split(';')
    dx = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    dy = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    '''
    if len(columns) > 1:
        try:
            x_value = float(1)
            y_value = float(2)
            dx = np.array(x_value)
            dy = np.array(y_value)
        except ValueError:
            pass
    '''
    h1.set_data(dx, dy)
    return h1,

def mostrar_datos_en_tiempo_real(data_queue):
    while True:
        data = data_queue.get()
        print(data)



def crear_grafico_desde_archivo_PID(nombre_archivo):
    df = pd.read_csv(nombre_archivo, delimiter=';')
    df['tiempo'] = df['tiempo'] / 2000
    print(df.columns)



    fig, axs = plt.subplots(2, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots

    fig2, axs2 = plt.subplots(1, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots

    # Subplot for Velocity
    if 'velocidad' in df.columns:
        axs[0].plot(df['tiempo'], df['velocidad'],'.-',label='velocidad')
    axs[0].set_xlabel('Time')
    axs[0].set_ylabel('Velocity Motor')
    axs[0].set_title('Velocity Arduino vs Time')
    axs[0].grid(True)
    axs[0].legend()

    if 'angulo' in df.columns:
        axs[1].plot(df['tiempo'], df['angulo'],'.', label = 'angulo')
    if 'error' in df.columns:
        axs[1].plot(df['tiempo'], df['error'],'.', label = 'error')
    if 'setPoint' in df.columns:
        axs[1].plot(df['tiempo'], df['setPoint'],'.', label = 'setPoint')
    axs[1].set_xlabel('Time')
    axs[1].set_ylabel('Error')
    axs[1].set_title('Error vs Time')
    axs[1].grid(True)
    axs[1].legend()

    #Subplot for error
    axs2.set_xlabel('Time')
    axs2.set_ylabel('Error Derivada Integral')
    axs2.set_title('Error vs Time')
    axs2.grid(True)

    if 'angulo' in df.columns:
        axs2.plot(df['tiempo'], df['angulo'], 'b.-', label='angulo')


    if 'error' in df.columns:
        axs2.plot(df['tiempo'], df['error'], 'b.-', label='Error')

    if 'integral' in df.columns:
        axs2.plot(df['tiempo'], df['integral'], 'g.-', label='integral')

    if 'derivada' in df.columns:
        axs2.plot(df['tiempo'], df['derivada'], 'r.-', label='derivada')
        axs2.grid(True)

    axs2.legend()

    fig.tight_layout()
    plt.show()



def calcular_seno_y_mostrar_entero(angulo_grados):
    # Convierte el ángulo de grados a radianes, ya que math.sin() espera radianes
    for paso in range(0, 200):
        angulo_grados = 0.45*paso
        angulo_radianes = math.radians(angulo_grados)

        # Calcula el seno del ángulo en radianes
        seno = math.sin(angulo_radianes)

        # Multiplica el seno por 1000
        resultado = seno * 1000

        # Imprime el resultado como un número entero
        print(int(resultado), end=',')


if __name__ == "__main__":
    crear_grafico_desde_archivo_PID('datos/datos_16.txt')
    '''
    port = "COM8"  # Reemplaza "COMX" con el nombre del puerto serie
    baud_rate = 115200  # Ajusta la velocidad de baudios según tu configuración

    output_folder = "datos"  # Carpeta para almacenar los archivos TXT

    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    ser = serial.Serial(port, baud_rate)

    # data_queue = queue.Queue()

    fig = plt.figure(figsize=(10, 8))
    plt.xlim(0, 15)
    plt.ylim(0, 15)
    h1, = plt.plot(0, 0)

    # line_ani = animation.FuncAnimation(fig, update_line, fargs=(h1, data_queue), interval=50, blit=False, save_count=10)

    # plt.show()

    data_queue = queue.Queue()

    file_number = 1
    while True:
        filename = f'datos/datos_{file_number}.txt'
        if not os.path.exists(filename):
            break
        file_number += 1
    file_path = f'datos/datos_{file_number}.txt'

    # Inicia los hilos para leer, guardar y mostrar datos
    read_thread = threading.Thread(target=leer_datos_serie, args=(ser, data_queue))
    save_thread = threading.Thread(target=guardar_datos_en_archivo, args=(data_queue, file_path))
    display_thread = threading.Thread(target=mostrar_datos_en_tiempo_real,
                                      args=(data_queue,))  # Agrega una coma después de (data_queue,)

    read_thread.start()
    save_thread.start()
    display_thread.start()

    try:
        read_thread.join()
        save_thread.join()
        display_thread.join()
    except KeyboardInterrupt:
        # Detén los hilos cuando se presiona Ctrl+C
        ser.close()
'''
    #calcular_seno_y_mostrar_entero(200)