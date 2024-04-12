from tkinter import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
import numpy as np
import serial as sr
import threading
import random
import time

datax = [1]
datay = [0]
s = None
stop = [False]

t_values = [0,0]
e_values = [0,0]
i_values = [0,0]
d_values = [0,0]
set_values = [0,0]
valAnt = 0

def enviar_datos():
    kp = entKp.get()  # Obtener el valor de Kp del campo de entrada
    ki = entKi.get()  # Obtener el valor de Ki del campo de entrada
    kd = entKd.get()  # Obtener el valor de Kd del campo de entrada
    
    if(not kp.replace('.','').replace('-','').isdigit()):
        kp=0
    if(not ki.replace('.','').replace('-','').isdigit()):
        ki=0
    if(not kd.replace('.','').replace('-','').isdigit()):
        kd=0
    
    # Convertir los valores a bytes
    mensaje = f"{kp} {ki} {kd}\r\n".encode()  # Convertir a una cadena y luego a bytes
    
    # Enviar el mensaje a través del puerto serie
    s.write(mensaje)
    print('Mensaje enviado:', mensaje)

def start_connection():
    global s
    global stop
    try:
        s = sr.Serial('COM14', 250000)
        hilo1 = threading.Thread(target=mi_hilo,args=(stop,))
        hilo1.start()
        start_connection.config(bg='light green')
    except Exception as e:
        print('error')
    

#datay.append(np.random.uniform(0, 5))
#datax.append(datax[-1]+1)
def start_plot():
    global valAnt
    if(len(t_values)>77):
        if(t_values[-1]>valAnt+5000):
            valAnt = t_values[-1]
            ax.clear()
            ax.set_xlim(t_values[-75], t_values[-1]+5000)
            ax.set_ylim(-6000, 6000)
        ax.plot(t_values[-75:-1],e_values[-75:-1], color="blue")
        ax.plot(t_values[-75:-1],set_values[-75:-1], color="green")
    else:
        #print(datax[-1],datay[-1])
        ax.plot(t_values[0:],e_values[0:], color="blue")
        ax.plot(t_values[-75:-1],set_values[-75:-1], color="green")
    canvas.draw_idle()
    main_window.after(100, start_plot) # in milliseconds, 1000 for 1 second


def mi_hilo(stop):
    print("Hilo", "iniciado")
    while not stop[0]:
        a = s.readline()
        try:
            a = a.decode()
        except Exception as e:
            print('fallo')
            a=""
        a = a.replace('\n',';')
        data_values = a.split(';')
        if len(data_values) == 6: #lleva un mas por uno que se agrega al final
            if data_values[0].isdigit() and data_values[1].lstrip('-').isdigit() and data_values[2].lstrip('-').lstrip('-').isdigit() and data_values[3].lstrip('-').isdigit() and data_values[4].lstrip('-').isdigit():
                t_values.append(int(data_values[0][0:]))
                if not data_values[1][0:] == e_values[-1]:
                    e_values.append(int(data_values[1][0:]))
                i_values.append(int(data_values[2][0:]))
                d_values.append(int(data_values[3][0:]))
                set_values.append(int(data_values[4][0:]))
                #print(t_values[-1],e_values[-1],set_values[-1])
        else:
            print(a)
        if(len(t_values)>78):
            del t_values[-78]
            del e_values[-78]
            del set_values[-78]

main_window = Tk()
main_window.configure(background='light blue')
main_window.title("ECG-LArdmon")
main_window.geometry('800x700')
main_window.resizable(width=False, height=False)




plotting_frame = LabelFrame(main_window, text='Real Time', bg='white', width=300, height=440, bd=5, relief=SUNKEN)
controls_frame = LabelFrame(main_window, text='Controls', background='light grey', height=150)

controls_frame.pack(fill='both', expand='1', side=TOP, padx=20, pady=10)
plotting_frame.pack(fill='both', expand='yes', side=BOTTOM, padx=20)



controls_frame.rowconfigure(0, weight=1)
controls_frame.columnconfigure(0, weight=1)
controls_frame.rowconfigure(1, weight=1)
controls_frame.columnconfigure(1, weight=1)
controls_frame.rowconfigure(2, weight=1)
controls_frame.columnconfigure(2, weight=1)


exit_button = Button(controls_frame, text='Close', width=10, height=2, borderwidth=3, command=main_window.destroy)
exit_button.grid(row=0, column=2, padx=5,pady=5)

frm_connection = Frame(master=controls_frame,background='light grey',borderwidth=1)
start_connection = Button(frm_connection, text='Start connection', width=16, height=1, borderwidth=3, command=start_connection,bg='light coral')
start_button = Button(frm_connection, text='Start Monitoring', width=16, height=1, borderwidth=3, command=start_plot)
start_connection.pack()
start_button.pack()
frm_connection.grid(row=0, column=0, padx=5, pady=5)

frm_PID = Frame(master=controls_frame,background='light grey',borderwidth=1)
frm_PID.rowconfigure(0, weight=1)
frm_PID.columnconfigure(0, weight=1)
frm_PID.rowconfigure(1, weight=1)
frm_PID.columnconfigure(1, weight=1)
frm_PID.rowconfigure(2, weight=1)
frm_PID.columnconfigure(2, weight=1)

# Agregar tres campos de entrada de texto
lbl_Kp = Label(master=frm_PID, text="Kp",background='light grey')
lbl_Kp.grid(row=0, column=1, padx=5)
entKp = Entry(frm_PID, width=15)
entKp.grid(row=1, column=1, padx=5, pady=5)

lbl_Ki = Label(master=frm_PID, text="Ki",background='light grey')
lbl_Ki.grid(row=0, column=2, padx=5, pady=1)
entKi = Entry(frm_PID, width=15)
entKi.grid(row=1, column=2, padx=5)

lbl_Kd = Label(master=frm_PID, text="Kd",background='light grey')
lbl_Kd.grid(row=0, column=3, padx=5)
entKd = Entry(frm_PID, width=15)
entKd.grid(row=1, column=3, padx=5, pady=5)


# Agregar un botón de envío debajo de los campos de entrada y centrado
enviar_button = Button(frm_PID, text='Enviar', width=10, height=1, borderwidth=1, command=enviar_datos)
enviar_button.grid(row=2, column=2, pady=5)

frm_PID.grid(row=0,column=1)


fig = Figure(dpi=100)
ax = fig.add_subplot(111)
ax.set_title("Electrocadiogram")
ax.set_xlabel("Time(Sec)")
ax.set_ylabel("Voltage(mV)")
ax.set_xlim(-50,1000)
ax.set_ylim(-6000, 6000)
#ax.grid(b=True, which='major', color='#666666', linestyle='-')
ax.minorticks_on()
#ax.grid(b=True, which='minor', color='#666666', linestyle='-', alpha=0.2)
canvas = FigureCanvasTkAgg(fig, master=plotting_frame)
canvas.get_tk_widget().place(x = 0, y = 0, width = 600, height = 420)
canvas.draw()

data=[]
for i in range(100):
    data.append(i)
print(data[-75:-1])

stop = [False]

main_window.mainloop()

stop[0] = True