import matplotlib.pyplot as plt

# Ejemplo de datos con el formato dado
data = "T000000009;A-0000;V6150;d0003"

# Separar los datos en una lista de strings
data_values = data.split(';')

# Extraer los valores numéricos de los datos
t_values = int(data_values[0][1:])  # Ignorar la 'T' y convertir a entero
a_values = int(data_values[1][1:])  # Ignorar la 'A' y convertir a entero
v_values = int(data_values[2][1:])  # Ignorar la 'V' y convertir a entero
d_values = int(data_values[3][1:])  # Ignorar la 'd' y convertir a entero

# Crear el gráfico
plt.figure(figsize=(10, 6))
plt.plot(t_values, a_values, label='A')
plt.plot(t_values, v_values, label='V')
plt.plot(t_values, d_values, label='D')

# Configurar etiquetas y título
plt.xlabel('T')
plt.ylabel('Valores')
plt.title('Gráfico de Valores')
plt.legend()

# Mostrar el gráfico
plt.show()
