#!/usr/bin/python
#-*- encoding: utf-8 -*-
import socket

host = "my-esp32.local" # mDNS hostname
port = 9876

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect((host, port))

client.send("take picture")

response = client.recv(1024)

print response
