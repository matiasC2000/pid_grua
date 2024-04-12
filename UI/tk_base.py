import tkinter as tk

class LabelPanelBase(tk.LabelFrame):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Configuración adicional del LabelPanelBase aquí
        self.background='light grey'

    def setEcuacion(self,ecuacion):
        pass

    def cambiarAEcuacion(self,ecuacion):
        pass

class LabelBase(tk.Label):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.background='light grey'
        
    def setEcuacion(self,ecuacion):
        pass

    def cambiarAEcuacion(self,ecuacion):
        pass