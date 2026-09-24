import serial
import socket
import threading

#Port of Arduino (Arduino IDE → Tools → Port),
serial_port = '/dev/tty.usbmodem48CA432F05642'  # ← our port
baud_rate = 115200

#IP (ipconfig),
target_ip = '10.128.26.47'  # ← IP
target_port = 5005 

#Initialisierung von Serial und UDP,
ser = serial.Serial(serial_port, baud_rate)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#Automatisches Senden eines „r“ beim Start,
def send_reset():
    print("🌀 Orientierungsrückstellung (r) beim Start gesendet")
    ser.write(b'r')

#Ein separater Thread soll auf Konsoleneingaben warten,
def keyboard_listener():
    while True:
        user_input = input()
        if user_input.strip().lower() == 'r':
            ser.write(b'r')
            print("🔄 Manuell gesendete Orientierungsrückstellung (r)")

#Start den Key Listener,
threading.Thread(target=keyboard_listener, daemon=True).start()

#Erstes Zurücksetzen beim Starten,
send_reset()

print("📤 Senden von Daten an Unity über UDP...")
while True:
    try:
        line = ser.readline().decode('utf-8').strip()
        sock.sendto(line.encode(), (target_ip, target_port))
        print(f"➡️ {line}")
    except Exception as e:
        print(f"❌ Fehler: {e}")
