#!/usr/bin/python

import socket
import sqlite3
import time
import signal
import sys

def signal_handler(signal, frame):
        print('exitting')
        conn.close()
        sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

conn = sqlite3.connect('measurements.db')
c = conn.cursor()
conn.execute('pragma foreign_keys=ON')

UDP_IP = ""
UDP_PORT = 1234

sock = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
        data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
        print "received message from {0}:{1}".format(data,addr)
        parts=data.split('-')
        if len(parts)!=4:
            print "Malformed message"
            print data
        location=parts[0]
        room=parts[1]
        measurement_type=parts[2]
        measurement=parts[3]
        if location=="" or room=="" or measurement_type=="" or measurement=="":
            print "Malformed message"
            print data
        print("Location: {0}\nroom: {1}\nmeasurement type: {2}\nmeasurement: {3}".format(location, room, measurement_type, measurement))
        sensor_id=None
        for row in c.execute(" SELECT * FROM sensors WHERE sensors.name=? and sensors.property=?",(room,location)):
            sensor_id=row[0]
            break
        if sensor_id==None:
            print "could not find sensor with name {0} and location {1}".format(room, location)
        else:
            c.execute("INSERT INTO measurements(time, sensor, measurement, measurement_type) VALUES(?,?,?,?)", (int(round(time.time())), sensor_id, measurement, measurement_type))
            conn.commit()
            for row in c.execute("SELECT * FROM measurements"):
                print row 
# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
conn.close()
