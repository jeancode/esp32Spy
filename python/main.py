
import socket
import pyaudio
import numpy as np
import wave


# Definir la dirección IP y el puerto UDP para la comunicación
UDP_IP = "0.0.0.0"
UDP_PORT = 12345

# Definir el tamaño del buffer para la recepción de datos UDP
BUFFER_SIZE = 1024

# Inicializar la biblioteca PyAudio
p = pyaudio.PyAudio()

# Abrir un flujo de audio de salida con las características deseadas
stream = p.open(format=pyaudio.paInt32,
                channels=1,
                rate=48000,
                output=True)

# Crear un socket UDP y unirlo a la dirección y puerto especificados
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# Definir el nombre del archivo de salida WAV
WAVE_OUTPUT_FILENAME = "output.wav"

# Abrir el archivo de salida WAV en modo escritura
wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')

# Establecer las características del archivo de salida WAV
wf.setnchannels(1)
wf.setsampwidth(4)
wf.setframerate(48000)

# Imprimir un mensaje indicando que se está esperando datos de audio
print("Esperando datos de audio...")

while True:
    # Recibir un paquete de datos UDP en el buffer de tamaño BUFFER_SIZE
    data, addr = sock.recvfrom(BUFFER_SIZE)

    # Convertir el buffer de bytes en un arreglo de números enteros de 32 bits utilizando NumPy
    samples = np.frombuffer(data, dtype=np.int32)

    # Aplicar un factor de ganancia a los datos de audio
    GAIN = 2.0
    samples = np.clip(samples * GAIN, np.iinfo(np.int32).min, np.iinfo(np.int32).max)

    # Escribir los datos de audio procesados en el flujo de audio de salida como bytes
    stream.write(samples.astype(np.int32).tobytes())

    # Escribir los datos de audio procesados en el archivo WAV de salida
    wf.writeframes(samples.astype(np.int32).tobytes())

    # Verificar si se ha ingresado la letra 'q' para salir del programa
    if ord(chr(data[0])) == ord('q'):
        break

    #muestra algunos datos para visulizar que se esta resividno
    #print(f"Recibiendo datos de audio de {addr} -_samples: {samples.shape}")

    #muestra  3 valores en formato HEX
    print(f"Valores HEX: {' | '.join(f'{b:#04x}' for b in data[0:10])}")







# Detener y cerrar el flujo de audio de salida
stream.stop_stream()
stream.close()

# Terminar la instancia de PyAudio
p.terminate()

# Cerrar el archivo de salida WAV
wf.close()


