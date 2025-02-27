#!/usr/bin/python
#-*- encoding: utf-8 -*-
import argparse
import socket

if __name__=='__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--host', help='tcp host', default="esp32-camera.local")
	parser.add_argument('--port', type=int, help='tcp port', default=49876)
	args = parser.parse_args()
	print("args.host={}".format(args.host))
	print("args.port={}".format(args.port))

	client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client.connect((args.host, args.port))
	client.send(b'take picture')
	response = client.recv(1024)
	client.close()
	if (type(response) is bytes):
		response=response.decode('utf-8')
	print(response)
