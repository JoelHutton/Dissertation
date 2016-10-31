#!/usr/bin/python

import sqlite3
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

while True:
    command=raw_input("what would you like to add/remove?(p/s/h/a/e/q):")
    if command=="p":
        print "properties(name,property_type,contact,notes)"
        for row in c.execute("SELECT * FROM properties"):
            print row
        if raw_input("add or delete (a/d):")=="d":
            c.execute("DELETE FROM properties WHERE name=?", (raw_input("name:"),)) 
        else:
            c.execute("INSERT INTO properties(name,property_type,contact,notes) VALUES (?,?,?,?)", (raw_input("name:"),raw_input("property type:"),raw_input("contact:"),raw_input("notes:"))) 
        conn.commit()
        print "properties(name,property_type,contact,notes)"
        for row in c.execute("SELECT * FROM properties"):
            print row
    elif command=="s":
        print "sensors(property,name,room_type)"
        for row in c.execute("SELECT * FROM sensors"):
            print row
        if raw_input("add or delete (a/d):")=="d":
            c.execute("DELETE FROM sensors WHERE id=?", (raw_input("id:"),)) 
        else:
            c.execute("INSERT INTO sensors(property,name,room_type) VALUES (?,?,?)", (raw_input("property:"),raw_input("sensor name:"),raw_input("room type:"))) 
        print "sensors(property,name,room_type)"
        for row in c.execute("SELECT * FROM sensors"):
            print row
        conn.commit()
    elif command=="a":
        print "properties(name,property_type,contact,notes)"
        for row in c.execute("SELECT * FROM properties"):
            print row
        print "sensors(property,name,room_type)"
        for row in c.execute("SELECT * FROM sensors"):
            print row
    elif command=="q":
        for row in conn.execute(raw_input("query:")):
            print row
        conn.commit()
    elif command=="e":
        conn.close()
        exit(0)
    else:
        print "type p for property,r for room,s for sensor, e to exit, a to display all, q to directly type query or  h to see this"
    
