import subprocess
import socket
import json

HOST = '192.168.0.15'
PORT = 9000

# manual = subprocess.Popen(["python", "mtest.py"])
manual = subprocess.Popen(["python", "obs.py", "drive", "--js"])
isPilot = False

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(5)
print('Server started at: %s:%s' % (HOST,PORT))

conn,addr = s.accept()
print('Connected by ',str(addr))

while True:
    recv_data=json.loads(conn.recv(256).decode())
    isCar = recv_data['car']
    isObstacle = recv_data['obs']
    distance = recv_data['dist']
    
    print('Received data: ',recv_data)
    if isCar & isObstacle:
        # print('Obstacle exists!')
        if isPilot:
            pilot.terminate()
            # manual = subprocess.Popen(["python", "mtest.py"])
            manual = subprocess.Popen(["python", "js.py"])
            isPilot = False
    elif isCar & ~isObstacle:
        if isPilot==False:
            manual.terminate()
            # pilot = subprocess.Popen(["python", "ptest.py"])
            pilot = subprocess.Popen(["python", "manage.py", "drive", "--model", "models/pilot1.h5"])
            isPilot = True
    else: # Car not in attention range
        if isPilot==False:
            manual.terminate()
            # pilot = subprocess.Popen(["python", "ptest.py"])
            pilot = subprocess.Popen(["python", "manage.py", "drive", "--model", "models/pilot1.h5"])
            isPilot = True