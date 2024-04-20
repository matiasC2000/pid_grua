from tkinter import * 
from panel_control import PanelControl
from panel_dibujo import PanelDibujo
from datos import InterfaceDatos
from variablesGlobales import *

import os
import pandas as pd

from matplotlib import pyplot as plt
import numpy as np
from abc import ABC, abstractmethod
import threading
import serial as sr

from math import tau
import time


class PanelPrincipal(Tk):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Configuración adicional del panel principal aquí
        self.title("La grua")
        self.configure(background='light blue')
        self.resizable(width=True, height=True)
        self.geometry('800x700')

        self.protocol("WM_DELETE_WINDOW", self.cerrar_ventana)

        #agrego los paneles necesarios
        self.controls_frame = PanelControl(self)
        self.controls_frame.grid(row=0, column=0, sticky="ew")

        #panel de grafico, tambien tengo que cambiar el color del boton
        self.grafico = PanelDibujo(self)
        self.grafico.plotting_frame.grid(row=1, column=0,columnspan=3,sticky="ew")


        self.mainloop()

        
    def cerrar_ventana(self):
        print("detener")

        # Detener las operaciones en segundo plano aquí
        datos = InterfaceDatos()
        datos.detener()

        #self.controls_frame.destroy()
        #self.grafico.destroy()

        # Cerrar la ventana
        self.destroy()
        
    def setEcuacion(self,ecuacion):
        self.grafico.cambiarAEcuacion(ecuacion)
        self.controls_frame.cambiarAEcuacion(ecuacion)
        
    def cambiarAEcuacion(self,ecuacion):
        pass



# Ejemplo de uso
if __name__ == "__main__":
    ventamain = PanelPrincipal()