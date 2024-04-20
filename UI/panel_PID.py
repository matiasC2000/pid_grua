# Clase para el panel PID
from tkinter import *
from tkinter.ttk import Combobox
from tk_base import LabelPanelBase
from tk_base import LabelBase
from datos import InterfaceDatos
from slider import Slider

class PanelPID(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs,borderwidth=1)
        # Configuración adicional del PanelPID aquí

        self.padre = args[0]

        # Por ejemplo, podrías establecer un texto de título para el LabelBaseFrame
        self.configure(text="Panel PID")

        #creo la grilla 1x3
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        self.columnconfigure(1, weight=1)
        self.rowconfigure(2, weight=1)
        self.columnconfigure(2, weight=1)

        # Agregar tres campos de entrada de texto
        lbl_Kp = LabelBase(master=self, text="Kp")
        lbl_Kp.grid(row=0, column=1, padx=5)
        self.entKp = Entry(self, width=15)
        self.entKp.insert(0, "0.0")
        self.entKp.grid(row=1, column=1, padx=5, pady=5)
        
        lbl_Ki = LabelBase(master=self, text="Ki")
        lbl_Ki.grid(row=0, column=2, padx=5, pady=1)
        self.entKi = Entry(self, width=15)
        self.entKi.insert(0, "0.0")
        self.entKi.grid(row=1, column=2, padx=5)
        
        lbl_Kd = LabelBase(master=self, text="Kd")
        lbl_Kd.grid(row=0, column=3, padx=5)
        self.entKd = Entry(self, width=15)
        self.entKd.insert(0, "0.0")
        self.entKd.grid(row=1, column=3, padx=5, pady=5)

        self.ecucacion = Combobox(
            self,
            state="readonly",
            values=["Basico", "SinSeno", "Posición"]
        )
        self.ecucacion.set("Basico")
        self.ecucacion.grid(row=2, column=1, padx=5, pady=5)

        # Agregar un botón de envío debajo de los campos de entrada y centrado
        enviar_button = Button(self, text='Enviar', width=10, height=1, borderwidth=1, command=self.enviar_datos)
        enviar_button.grid(row=2, column=2, pady=5)


    def enviar_datos(self):
        # Aquí puedes agregar la lógica para enviar los datos
        # Por ejemplo, puedes obtener los valores de los campos de entrada entKp, entKi y entKd
        kp = self.entKp.get()
        ki = self.entKi.get()
        kd = self.entKd.get()
        ecuacion = self.ecucacion.current()
        print(kp,ki,kd,ecuacion)
        dato = InterfaceDatos()
        dato.actualizar_coef_PID(kp,ki,kd,ecuacion)
        if(ecuacion==2 and dato.stateSimulacion):
            slider_instance = Slider()
        #llama a la clase que se encarga de enviar
        self.setEcuacion(self.ecucacion.get())

    def setEcuacion(self,ecuacion):
        self.padre.setEcuacion(ecuacion)