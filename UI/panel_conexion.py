from tk_base import *
from tkinter import *
from datos import InterfaceDatos

class PanelConexion(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Configuración adicional del ControlFrame aquí
        self.ventana = args[0]

        self.text_cambiar='Simulacion'

        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        self.rowconfigure(2, weight=1)
        
        # Creamos ambos paneles
        self.panel_arduino = PanelArduino(self)
        self.panel_simulacion = PanelSimulacion(self)
        
        # Inicialmente mostramos el panel de Arduino y ocultamos el de Simulación
        self.panel_arduino.grid(row=0, column=0)
        self.panel_simulacion.grid(row=0, column=0)
        self.panel_simulacion.grid_forget()
        
        # Botón para cambiar entre paneles
        self.change_button = Button(self, text=self.text_cambiar, width=10, height=1, borderwidth=1, command=self.change)
        self.change_button.grid(row=1, column=0, padx=5, pady=5)

        self.start()

    def change(self):
        # Verificar cuál panel está actualmente visible y alternar su estado de visibilidad
        datos = InterfaceDatos()
        if self.panel_arduino.winfo_ismapped():
            datos.cambiar_simulacion()
            self.panel_arduino.grid_forget()
            self.panel_simulacion.grid(row=0, column=0, sticky="nsew")
            self.change_button.config(text='Arduino')
        else:
            datos.cambiar_arduino()
            self.panel_simulacion.grid_forget()
            self.panel_arduino.grid(row=0, column=0, sticky="nsew")
            self.change_button.config(text='Simulacion')

    def start(self):
        datos = InterfaceDatos()
        if datos.get_estado():
            self.panel_arduino.connect_button.config(bg='light green')
        else:
            self.panel_arduino.connect_button.config(bg='light coral')
        self.ventana.after(100, self.start) # in milliseconds, 1000 for 1 second



class PanelArduino(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        self.lbl_port = LabelBase(master=self, text="port")
        self.lbl_port.grid(row=0, column=0, padx=5)

        self.ent_port = Entry(self, width=15)
        self.ent_port.insert(0, "COM14")
        self.ent_port.grid(row=1, column=0, padx=5, pady=5)

        # Botón para cambiar entre paneles
        self.connect_button = Button(self, text='connect',width=10, height=1, borderwidth=1, command = self.connect)
        self.connect_button.grid(row=2, column=0, padx=5, pady=5)

    def connect(self):
        self.port = self.ent_port.get()
        datos = InterfaceDatos()
        datos.inicio(self.port)

class PanelSimulacion(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        self.lbl_pos = LabelBase(master=self, text="pos")
        self.lbl_pos.grid(row=0, column=0, padx=5)

        self.ent_pos = Entry(self, width=15)
        self.ent_pos.grid(row=1, column=0, padx=5, pady=5)

        # Botón para cambiar entre paneles
        self.set_button = Button(self, text='set pos',width=10, height=1, borderwidth=1, command=self.Set_pos)
        self.set_button.grid(row=2, column=0, padx=5, pady=5)

    def Set_pos(self):
        self.pos = self.ent_pos.get()
        print(self.pos)
        datos = InterfaceDatos()
        datos.inicio(self.pos)