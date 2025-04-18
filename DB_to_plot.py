#! /usr/bin/python

import time
import datetime
import mysql.connector as mysql
import matplotlib
matplotlib.use("gtk3agg")
import matplotlib.pyplot as plt
import numpy as np

con = mysql.connect(host="localhost",user="mesres_usr",password="mesres_password",database="mesres")

cur = con.cursor()

print('Connected...')

dataTemp = []
dataTTimestamp = []
dataHum = []
dataHTimestamp = []

print('Printing from DB_to_diagram')

cur.execute("SELECT * FROM DHT11hum")
print("From database mesres humidity")
for row in cur.fetchall():
    print(row)
    dataHum.append(row[1])
    dataHTimestamp.append(row[4])
    print("Timestamp ", row[4])
print('---------------------------------------------------------')
    
cur.execute("SELECT * FROM DHT11temp")
print("From database mesres temperature")
for row in cur.fetchall():
    print(row)
    dataTemp.append(row[1])
    dataTTimestamp.append(row[4])
    print("Timestamp ", row[4])
print('---------------------------------------------------------')

print('Plot!')

plt.figure(1)

plt.subplot(211)

# plt.xticks( rotation=25 )
ax=plt.gca()
xfmt = matplotlib.dates.DateFormatter('%Y-%m-%d %H:%M')
ax.xaxis.set_major_formatter(xfmt)

plt.plot(dataTTimestamp,dataTemp, 'o-')
plt.xlabel('Time stamp')
plt.ylabel('Temperature [C]')
plt.title('Temperature measured by DHT11')
plt.grid(True)
# plt.savefig("test.png")

plt.subplot(212)

# plt.xticks( rotation=25 )
ax=plt.gca()
xfmt = matplotlib.dates.DateFormatter('%Y-%m-%d %H:%M')
ax.xaxis.set_major_formatter(xfmt)

plt.plot(dataHTimestamp,dataHum, 'o-')
plt.xlabel('Time stamp')
plt.ylabel('Humidity [%]')
plt.title('Humidity measured by DHT11')
plt.grid(True)
# plt.savefig("test.png")
plt.show()
