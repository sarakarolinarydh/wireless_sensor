#! /usr/bin/python

import paho.mqtt.client as mqtt
import time
import datetime
import json
import mysql.connector as mysql

broker = "192.168.1..."

con = mysql.connect(host="localhost",user="mesres_usr",password="mesres_password",database="mesres")

cur = con.cursor()

print('Connected...')

# The callback for when the client receives a CONNACK response from the server
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("sensor/DHT11")

# The callback for when a PUBLISH message is received from the server
def on_message(client, userdata, msg):
    try:
        print(msg.topic+" "+str(msg.payload))
        jsonLoads = json.loads(msg.payload)
        h = jsonLoads.get("values")[0] # Humidity
        t = jsonLoads.get("values")[1] # Temperature

        c = datetime.datetime.now()
        print(c.strftime('%Y-%m-%d'))
        print(c.strftime('%H:%M:%S'))
        print(c)

        cur.execute("INSERT INTO DHT11hum (value, date, time, sensor_timestamp) VALUES (%s, %s, %s, %s)", (h, c, c, c))
        cur.execute("INSERT INTO DHT11temp (value, date, time, sensor_timestamp) VALUES (%s, %s, %s, %s)", (t, c, c, c))

        con.commit()
    
    except Exception as E:
        print (E)
    	
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set("username", "password_sinking_sun")
client.connect(broker, 1883, 60)

client.subscribe("sensor/DHT11")               

client.loop_forever()
