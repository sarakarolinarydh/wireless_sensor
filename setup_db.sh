#!/bin/bash

mysql -u root -p'my_root_password' << EOF
CREATE OR REPLACE USER mesres_usr@localhost IDENTIFIED BY "mesres_password";
CREATE DATABASE mesres;
USE mesres3;
CREATE TABLE DHT11temp (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, value DECIMAL(4,2), date DATE, time TIME, sensor_timestamp DATETIME, timestamp TIMESTAMP );
CREATE TABLE DHT11hum (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, value DECIMAL(4,2), date DATE, time TIME, sensor_timestamp DATETIME, timestamp TIMESTAMP );
GRANT ALL PRIVILEGES ON mesres.* TO "mesres_usr"@"localhost";
FLUSH PRIVILEGES;
EOF
