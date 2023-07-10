#!/usr/bin/python
#-*- encoding: utf-8 -*-
import socket

host = "esp32-camera.local" # esp32 hostname
port = 49876

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect((host, port))

client.send(b'take picture')

response = client.recv(1024)

client.close()

print(response)
