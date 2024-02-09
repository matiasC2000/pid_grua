import socket

# Configuración del servidor UDP
host = '0.0.0.0'  # Escucha en todas las interfaces de red
port = 12345  # Puerto UDP arbitrario

# Crear un socket UDP
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Enlazar el socket al host y puerto
udp_socket.bind((host, port))

print(f"Escuchando en {host}:{port}")

while True:
    # Recibir datos del cliente
    data, addr = udp_socket.recvfrom(1024)  # El tamaño del búfer es 1024 bytes, puedes ajustarlo según tus necesidades

    # Imprimir los datos recibidos
    print(f"Recibido de {addr}: {data.decode('utf-8')}")

# Cerrar el socket al salir del bucle (esto nunca se ejecutará en este ejemplo)
udp_socket.close()

