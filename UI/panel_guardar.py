from tk_base import *
from tkinter import *
from tkinter import filedialog
from datos import InterfaceDatos

class PanelGuardar(LabelPanelBase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.archivo=''


        self.configure(text="Panel guardar")

        #creo la grilla 2x2
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        self.columnconfigure(1, weight=1)

        enviar_button = Button(self, text ='Start record', width=10, height=1, borderwidth=1, command=self.Start_record)
        enviar_button.grid(row=0, column=0, pady=5)

        enviar_button = Button(self, text='Stop record', width=10, height=1, borderwidth=1, command=self.Stop_record)
        enviar_button.grid(row=1, column=0, pady=5)


        boton_seleccionar = tk.Button(self, text="Seleccionar Archivo", command= self.Seleccionar_archivo)
        boton_seleccionar.grid(row=0,column=1,pady=10)

        enviar_button = Button(self, text='Abrir archivo', width=10, height=1, borderwidth=1, command=self.Abrir_archivo)
        enviar_button.grid(row=1, column=1, pady=5)


    def Abrir_archivo(self):
        print("Grafico lindo")
        datos = InterfaceDatos()
        datos.abrir_archivo()

    def Seleccionar_archivo(self):
        self.archivo = filedialog.askopenfilename()
        print(self.archivo)
        datos = InterfaceDatos()
        datos.seleccionar_ruta(self.archivo)
        datos.abrir_archivo()

    def Start_record(self):
        datos = InterfaceDatos()
        datos.inicio_guardar_datos()
        print('Start record')

    def Stop_record(self):
        datos = InterfaceDatos()
        datos.detener_guardar_datos()
        print('Stop record')