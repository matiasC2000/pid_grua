from tkinter import *
from datos import InterfaceDatos
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
from tk_base import *
from variablesGlobales import *

class PanelDibujo():
    def __init__(self, ventana):
        super().__init__()        
        self.valAnt=0

        self.nombres_ecuaciones=["Basico", "SinSeno", "Posición"]

        self.valores_ecuaciones = [
            (-1000, 1000, False),
            (-810, 810, False),
            (-1000, 15000, True)        #buscar cuales son los valores
        ]

        self.valor_ecuacion = self.valores_ecuaciones[0]

        print('start panelDibujo')

        #creo el primer plot
        self.plotting_frame = LabelPanelBase(ventana, text='Real Time', bg='white', width=300, height=440, bd=5, relief=SUNKEN)

        self.ventana = ventana

        self.generador = InterfaceDatos()

        self.fig = Figure(dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_title("Error vs time")
        self.ax.set_xlabel("Time(Sec)")
        self.ax.set_ylabel("Error")
        self.ax.set_xlim(-1,2)
        self.ax.set_ylim(self.valor_ecuacion[0], self.valor_ecuacion[1])
        self.ax.minorticks_on()
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.plotting_frame)
        self.canvas.get_tk_widget().place(x = 0, y = 0, width = 600, height = 420)
        self.canvas.draw()
        self.start_plot()


    def start_plot(self):
        array = self.generador.get_datos()

        if array != None:
            t_values = array[0]
            e_values = array[1]
            set_values = array[2] 

            if(t_values[-1] - t_values[0] > tiempoEnArreglo - 0.1):
                if(t_values[-1]>self.valAnt+2):
                    self.valAnt = t_values[-1]
                    self.ax.clear()
                    self.ax.set_xlim(t_values[0], t_values[-1]+10)
                    self.ax.set_ylim(self.valor_ecuacion[0], self.valor_ecuacion[1])

                self.ax.plot(t_values[0:-1],e_values[0:-1], color="blue")
                if self.valor_ecuacion[2]:
                    self.ax.plot(t_values[0:-1],set_values[0:-1], color="green")
            else:
                #print(datax[-1],datay[-1])
                self.ax.plot(t_values[0:],e_values[0:], color="blue")
                if self.valor_ecuacion[2]:
                    self.ax.plot(t_values[0:-1],set_values[0:-1], color="green")
            self.canvas.draw_idle()
        self.ventana.after(100, self.start_plot) # in milliseconds, 1000 for 1 second

    def setEcuacion(self,ecuacion):
        pass

    def cambiarAEcuacion(self,ecuacion):
        self.valor_ecuacion = self.valores_ecuaciones[self.nombres_ecuaciones.index(ecuacion)]