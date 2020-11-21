#!/usr/bin/python
#-*- encoding: utf-8 -*-
#https://ameblo.jp/oyasai10/entry-10590541669.html

from socket import *

HOST = ''
PORT = 9876
ADDRESS = "192.168.10.255" # for Broadcast

s = socket(AF_INET, SOCK_DGRAM)
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
s.bind((HOST, PORT))

s.sendto(b'take picture', (ADDRESS, PORT))

s.close()
