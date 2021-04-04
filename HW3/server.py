import socket
import json
import matplotlib.pyplot as plt

HOST='10.0.0.20'
PORT=8000

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(5)
print('Server started at: %s:%s' % (HOST,PORT))

while True:
	conn,addr=s.accept()
	print('Connected by ',str(addr))
	t=[]; h=[]; p=[]
	ax=[]; ay=[]; az=[]
	gx=[]; gy=[]; gz=[]
	mx=[]; my=[]; mz=[]
	buf=30
	
	while True:
		recv_data=json.loads(conn.recv(1024).decode())
		if len(t)>=buf:
			t.pop(0)
			h.pop(0)
			p.pop(0)
			ax.pop(0)
			ay.pop(0)
			az.pop(0)
			gx.pop(0)
			gy.pop(0)
			gz.pop(0)
			mx.pop(0)
			my.pop(0)
			mz.pop(0)
		t.append(recv_data['t'])
		h.append(recv_data['h'])
		p.append(recv_data['p'])
		ax.append(recv_data['ax'])
		ay.append(recv_data['ay'])
		az.append(recv_data['az'])
		gx.append(recv_data['gx'])
		gy.append(recv_data['gy'])
		gz.append(recv_data['gz'])
		mx.append(recv_data['mx'])
		my.append(recv_data['my'])
		mz.append(recv_data['mz'])

		print('Received data: ',recv_data)
		#print('Temperature: ',t)
		#print('Humidity: ',h)
		#print('Pressure: ',p)
		#print('Magneto x: ',mx)

		x=list(range(-len(t)+1,1))
		label=['x','y','z']
		a=[ax,ay,az]
		g=[gx,gy,gz]
		m=[mx,my,mz]
		plt.clf()
		plt.subplot(231)
		plt.plot(x,t)
		plt.xlabel('Time (s)')
		plt.ylabel('Temperature ('+chr(176)+'C)')
		plt.subplot(232)
		plt.plot(x,h)
		plt.xlabel('Time (s)')
		plt.ylabel('Humidity (%)')
		plt.subplot(233)
		plt.plot(x,p)
		plt.xlabel('Time (s)')
		plt.ylabel('Pressure (mBar)')
		plt.subplot(234)
		for i in range(len(label)):
			plt.plot(x,a[i],label=label[i])
			plt.xlabel('Time (s)')
			plt.ylabel('Accelerometer')
			plt.legend()
		plt.subplot(235)
		for i in range(len(label)):
			plt.plot(x,g[i],label=label[i])
			plt.xlabel('Time (s)')
			plt.ylabel('Gyroscope')
			plt.legend()
		plt.subplot(236)
		for i in range(len(label)):
			plt.plot(x,m[i],label=label[i])
			plt.xlabel('Time (s)')
			plt.ylabel('Magnetometer')
			plt.legend()
		plt.pause(0.001)