import tkinter as tk
from tk_base import LabelPanelBase

from panel_PID import PanelPID
from panel_guardar import PanelGuardar
from panel_conexion import PanelConexion

class PanelControl(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Configuración adicional del ControlFrame aquí
        self.ventana = args[0]
        
        #creando la grilla de 1x3
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)
        
        conexion = PanelConexion(self.ventana)
        conexion.grid(row=0, column=0, sticky="nsew")
        
        PID = PanelPID(self.ventana)
        PID.grid(row=0, column=1, sticky="nsew")
        
        guardar = PanelGuardar(self.ventana)
        guardar.grid(row=0, column=2, sticky="nsew")
    
    def setEcuacion(self,ecuacion):
        self.ventana.setEcuacion(ecuacion)

    def cambiarAEcuacion(self,ecuacion):
        pass