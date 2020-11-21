#!/usr/bin/python
#-*- encoding: utf-8 -*-
import socket

host = "192.168.10.110" # Your hostname
port = 9876

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect((host, port))

client.send(b'take picture')

response = client.recv(1024)

client.close()

print(response)
