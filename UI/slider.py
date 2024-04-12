from tkinter import *

from tkinter import *

class Slider():
    _instance = None

    def __new__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.root = Tk()
        self.root.title("Posicion de la carga")
        self.slider_value = 0
        self.slider_var = IntVar()
        self.slider = Scale(self.root, from_=0, to=1024, variable=self.slider_var, orient=HORIZONTAL, length=400)
        self.slider.set(1024/2)  # Valor predeterminado
        self.slider.pack()

    def open_window(self):
        self.root.mainloop()

    def get_slider_value(self):
        self.slider_value = self.slider_var.get()
        return self.slider_value

# Ejemplo de uso:
if __name__ == "__main__":
    slider_instance = Slider()
    slider_instance.open_window()
    print(slider_instance.get_slider_value())