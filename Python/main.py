import serial
import json
import os
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

def filtrar_archivo(archivo_entrada, archivo_salida):
    lineas_validas = []

    with open(archivo_entrada, 'r') as archivo:
        lineas = archivo.readlines()
    for linea in lineas:
        if es_formato_valido_archivo(linea):
            partes = linea.split()
            t = partes[0]
            e = partes[1]
            a = partes[2]
            v = partes[3]
            d = partes[4]
            linea_datos = f"{t};{e};{a};{v};{d}\n"
            lineas_validas.append(linea_datos)

    with open(archivo_salida, 'w') as archivo:
        archivo.writelines(lineas_validas)


def es_formato_valido_archivo(linea):
    #partes = linea.split(';')
    partes = linea.split()
    if len(partes) != 5:
        return False
    t_parte = partes[0]
    e_parte = partes[1]
    a_parte = partes[2]
    v_parte = partes[3]
    d_parte = partes[4]

    if not t_parte.startswith('T') or not t_parte[1:].isdigit():
        return False

    if not (e_parte.startswith('E+') or e_parte.startswith('E-')) or not e_parte[2:].isdigit():
        return False

    if not (a_parte.startswith('A+') or a_parte.startswith('A-')) or not a_parte[2:].isdigit():
        return False

    if not (v_parte.startswith('V+') or v_parte.startswith('V-')) or not v_parte[2:].isdigit():
        return False

    if not d_parte.startswith('d') or not d_parte[2:].isdigit():
        return False

    return True


def filtrar_formato_valido(nombre_archivo):
    lineas_validas = []
    with open(nombre_archivo, 'r') as archivo:
        for linea in archivo:
            if es_formato_valido(linea):
                lineas_validas.append(linea)

    with open(nombre_archivo, 'w') as archivo:
        archivo.writelines(lineas_validas)

def crear_grafico_desde_archivo(nombre_archivo):
    with open(nombre_archivo, 'r') as archivo:
        lineas = archivo.readlines()

    t_values = []
    a_values = []
    v_values = []
    d_values = []


    for linea in lineas:
        data_values = linea.strip().split(';')
        if len(data_values) == 4:
            t_values.append(int(data_values[0][1:]))

            a_value = int(data_values[1][1:]) if data_values[1][0] == 'A' else int(data_values[1][1:])
            a_values.append(a_value)

            v_value = int(data_values[2][1:].split(' ')[0]) if data_values[2][0].isdigit() else int(data_values[2][1:])
            v_values.append(v_value)

            d_values.append(int(data_values[3][1:]))

    '''
    plt.figure(figsize=(10, 6))
    plt.plot(t_values, a_values, label='A')
    #plt.plot(t_values, v_values, label='V')
    #plt.plot(t_values, d_values, label='D')

    plt.xlabel('T')
    plt.ylabel('Valores Angulo')
    plt.title('Gráfico de Valores')
    plt.legend()
    plt.grid(True)

    plt.show()
    '''
    fig, axs = plt.subplots(3, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots

    # Subplot for Velocity
    axs[0].plot(t_values, v_values,'.-')
    axs[0].set_xlabel('Time')
    axs[0].set_ylabel('Velocity Arduino')
    axs[0].set_title('Velocity Arduino vs Time')
    axs[0].grid(True)

    # Subplot for Angle
    axs[1].plot(t_values, a_values,'.')
    axs[1].set_xlabel('Time')
    axs[1].set_ylabel('Angle')
    axs[1].set_title('Angle vs Time')
    axs[1].grid(True)

    '''
    vPos_values = [0,0,0,0]
    for i in range(4, len(t_values)):
        prom = (a_values[i - 1]/2 + a_values[i - 2]/4 + a_values[i - 3]/8 + a_values[i - 4]/8)
        if (t_values[i] - t_values[i - 1]) != 0:
            vel = (prom - a_values[i]) / (t_values[i] - t_values[i - 1])
        else:
            vel = 0  # Manejar la división por cero

        vPos_values.append(vel)
    '''


    vPos_values = [0,0,0,0]
    prom = 0
    for i in range(4, len(t_values)):
        if (v_values[i] != 0):
            #prom = 0.5 * prom + 0.5 * v_values[i]
            vel = 100000/v_values[i]
            #vel = (a_values[i-1] - a_values[i]) / (t_values[i-1] - t_values[i])
            #vel = vel/2 + vPos_values[i-1]/4 +vPos_values[i-2]/8 + vPos_values[i-3]/8
            #vel2 = (a_values[i] - a_values[i+1]) / (t_values[i] - t_values[i+1])
            #vel = (vel1+vel2)/2
        else:
            vel = 0  # Manejar la división por cero

        vPos_values.append(vel)

    # Subplot for Angle

    # axs[2].plot(t_values, vPos_values,'.-')
    # axs[2].set_xlabel('Time')
    # axs[2].set_ylabel('Velocidad PosProceso')
    # axs[2].set_title('Velocidad PosProceso vs Time')
    # axs[2].grid(True)

    fig.tight_layout()
    plt.show()

def crear_grafico_desde_archivo_PID(nombre_archivo):
    with open(nombre_archivo, 'r') as archivo:
        lineas = archivo.readlines()

    t_values = []
    e_values = []
    a_values = []
    v_values = []
    d_values = []

    for linea in lineas:
        data_values = linea.split(';')
        if len(data_values) == 5:
            t_values.append(int(data_values[0][1:]))

            #e_value = int(data_values[1][1:]) if data_values[1][0] == 'E' else int(data_values[1][1:])
            e_values.append(int(data_values[1][1:]))

            #a_value = int(data_values[2][1:]) if data_values[2][0] == 'A' else int(data_values[2][1:])
            a_values.append(int(data_values[2][1:]))

            #v_value = int(data_values[3][1:]) if data_values[3][0] == 'V' else int(data_values[3][1:])
            v_values.append(int(data_values[3][1:]))

            d_values.append(int(data_values[4][1:]))

    fig, axs = plt.subplots(2, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots

    fig2, axs2 = plt.subplots(2, 1, figsize=(10, 8))  # 2 rows, 1 column of subplots

    # Subplot for Velocity
    axs[0].plot(t_values, v_values,'.-')
    axs[0].set_xlabel('Time')
    axs[0].set_ylabel('Velocity Motor')
    axs[0].set_title('Velocity Arduino vs Time')
    axs[0].grid(True)

    # Subplot for error
    axs[1].plot(t_values, e_values,'.')
    axs[1].set_xlabel('Time')
    axs[1].set_ylabel('Error')
    axs[1].set_title('Error vs Time')
    axs[1].grid(True)

    '''
    # Subplot for error
    axs[2].plot(t_values, a_values, '.')
    axs[2].set_xlabel('Time')
    axs[2].set_ylabel('Angulo')
    axs[2].set_title('Angulo vs Time')
    axs[2].grid(True)
    '''
    #Falta hacer filtro paso bajos para ver que onda


    #0.1159  *( a1= 1.0000   a2= 2.0000  a3=  1.0000)
    # b1 1.0000   b2 -0.8339   b3 0.2973

    #frecuencia de muestreo es de 50
    #frecuencia del pendulo 0.66666
    #tengo que buscar la ecuacion de frecuenciade un pendulo
    #1.0000    2.0000    1.0000 *  0.0017
    #b2 -1.8818   b3 0.8884

    #y(n)= a1X(n) + a2X(n-1) +a3X(n-2) - b1X(n) - b2X(n-1) - b2X(n-2)
    #los y(n) son vPos_values y los x(n) son los v_values
    # vel = 0.0017 * v_values[i] + 0.0017 * 2 * v_values[i-1] + 0.0017 * v_values[i-2]
    # vel = vel + 1.8818 * vPos_values[i-1] - 0.8884 * vPos_values[i-2]

    vPos_values = [0,0,0,0]
    iCorregido_values = [0, 0, 0, 0]
    prom = 0
    s = -548
    vel = 0
    correcion = 0
    t_cero = 0
    t_ant = 0
    v_ant = 0
    for i in range(4, len(t_values)):                                                        #1.0000   b2 -0.8339   b3 0.2973
        '''
        if t_values[i] > 440+t_ant:
            vel = v_ant - v_values[i]
            #prom = prom/2 + vel/2
            #axs2.axvline(x=t_values[i], color='gray', linestyle='--')
            if vel == 0 :
                t_cero = t_cero + t_values[i] - t_ant
            if vel != 0 :
                t_cero = 0
            if t_cero > 3100 * 0.5:
                correcion = - a_values[i]
                t_cero = 0
            t_ant = t_values[i]
            v_ant = v_values[i]
        '''
        s = s + e_values[i]
        iCorregido_values.append(s)
        #a_values[i] = a_values[i] - 22

        vPos_values.append(vel)

    for i,t in enumerate(t_values):
        t_values[i] = t*0.5/1000


    # Subplot for error
    axs2[0].plot(t_values, v_values,'b.-')
    axs2[0].set_xlabel('Time')
    axs2[0].set_ylabel('velociadd')
    axs2[0].set_title('velocidad arduino')
    axs2[0].grid(True)

    #Subplot for error
    axs2[1].plot(t_values, e_values,'b.-')
    axs2[1].set_xlabel('Time')
    axs2[1].set_ylabel('Error Derivada Integral')
    axs2[1].set_title('Error vs Time')
    axs2[1].grid(True)

    # Subplot for error
    #axs2[1].plot(t_values, d_values, 'r.-')#vPos_values
    axs2[1].plot(t_values, a_values, 'g.-')
    axs2[1].grid(True)

    axs2[1].plot(t_values, d_values, 'r.-')
    axs2[1].grid(True)


    # Subplot for Angle
    axs[1].plot(t_values, vPos_values,'r.-')
    axs[1].set_xlabel('Time')
    axs[1].set_ylabel('Error PosProceso')
    axs[1].set_title('Error PosProceso vs Time')
    axs[1].grid(True)


    fig.tight_layout()
    plt.show()


def cargar_configuracion():
    with open("config.json") as archivo:
        configuracion = json.load(archivo)
    return configuracion


def main():
    print("directorio Actual: ", os.getcwd())
    puerto = None
    try:
        configuracion = cargar_configuracion()
        puerto_nombre = configuracion["puerto"]
        velocidad = configuracion["velocidad"]


        puerto = serial.Serial(puerto_nombre, velocidad)

        while True:
            try:
                datos = puerto.readline().decode('utf-8')
            except UnicodeDecodeError:
                print("Error de decodificación, se omitieron los datos problemáticos.")
                continue
            print("Datos recibidos:", datos)

    except KeyboardInterrupt:
        print("Programa detenido por el usuario.")
    except Exception as e:
        print("Error:", e)
    finally:
        if puerto is not None:
            puerto.close()


def es_formato_valido(data):
    if len(data) >= 34:
        if data[1] == 'T':
            for i in range(2, 11):
                if not data[i].isdigit():
                    return False
            if data[13] == 'A':
                for i in range(15, 19):
                    if not data[i].isdigit():
                        return False
                if data[21] == 'V':
                    for i in range(23, 27):
                        if not data[i].isdigit():
                            return False
                    if data[29] == 'd':
                        for i in range(30, 34):
                            if not data[i].isdigit():
                                return False
        return True
    else:
        return False


def guardarenarchivo(puerto_serial, bps, nombre_archivo):
    serial_port = serial.Serial(puerto_serial, bps)
    archivo_salida = open(nombre_archivo, "w")

    try:
        while True:
            try:
                data = serial_port.readline().decode('utf-8')
            except UnicodeDecodeError:
                print("Error de decodificación, se omitieron los datos problemáticos.")
                continue
            archivo_salida.write(data)
            print(data)
            '''
            if es_formato_valido(data):
                t = data[1:11]
                a = data[13:19]
                v = data[21:27]
                d = data[29:34]

                linea_datos = f"{t};{a};{v};{d}\n"
                archivo_salida.write(linea_datos)
            else:
                print("Mensaje no válido, se omitió:", data)
            '''
    except KeyboardInterrupt:
        print("Lectura del puerto serie detenida.")
        serial_port.close()
        archivo_salida.close()

def update_line(num,h1,data_queue):
    #data = data_queue.get()
    #columns = data.split(';')
    dx = np.array([0,1,2,3,4,5,6,7,8,9])
    dy = np.array([0,1,2,3,4,5,6,7,8,9])
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


if __name__ == "__main__":
    #main()

    configuracion = cargar_configuracion()
    puerto_nombre = configuracion["puerto"]
    velocidad = configuracion["velocidad"]
    archivo_entrada = 'test-abajo3.txt'
    archivo_salida = 'test-abajo3-Filtrado.txt'
    guardarenarchivo(puerto_nombre, velocidad, archivo_entrada)
    '''
    data_queue = np.array([[0][0]])

    fig = plt.figure(figsize=(10, 8))
    plt.xlim(0, 15)
    plt.ylim(0, 15)
    h1, = plt.plot(0, 0)

    line_ani = animation.FuncAnimation(fig, update_line, fargs=(h1, data_queue), interval=50, blit=False, save_count=10)

    plt.show()
    '''
    #filtrar_archivo(archivo_entrada, archivo_salida)
    #crear_grafico_desde_archivo(archivo_salida)
    #crear_grafico_desde_archivo_PID(archivo_salida)