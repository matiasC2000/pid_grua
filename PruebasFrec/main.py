from scipy.signal import butter
from scipy import signal
import numpy as np
import matplotlib.pyplot as plt



def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    # tiempo duracion programa
    t_duration = np.linspace(0, 2, 50, False)

    # dos seniales con 20hz y 40hz
    # despues cambiar la de 20 por una de 0.66
    sign = np.sin(2 * np.pi * 0.66 * t_duration) #+ np.sin(2 * np.pi * 20 * t_duration)
    sign[30] = sign[30] + 2

    fig, (ax1) = plt.subplots(1, 1, sharex=True)
    ax1.plot(t_duration, sign)
    ax1.set_title('20 and 40 Hz Sinusoid')
    ax1.axis([0, 2, -2, 2.5])

    sos = butter(15, 20, btype="low", fs=50, output="sos")
    filtd = signal.sosfilt(sos, sign)

    fig, (ax2) = plt.subplots(1, 1, sharex=True)
    ax2.plot(t_duration, filtd)
    ax2.set_title('After applying 20 Hz high-pass filter')
    ax2.axis([0, 2, -2, 2.5])
    ax2.set_xlabel('Time (seconds)')
    plt.tight_layout()
    plt.show()


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
