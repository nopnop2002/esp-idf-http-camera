#!/usr/bin/python
#-*- encoding: utf-8 -*-
#
# Requirement library
# python3 -m pip install -U netifaces
#
import argparse
import socket
import netifaces

# Get IP address
for iface_name in netifaces.interfaces():
	iface_data = netifaces.ifaddresses(iface_name)
	ipList=iface_data.get(netifaces.AF_INET)
	#print("ip={}".format(ipList))
	ipDict = ipList[0]
	addr = ipDict["addr"]
	print("addr={}".format(addr))
	if (addr != "127.0.0.1"):
		myIp = addr

if __name__=='__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--port', type=int, help='tcp port', default=49876)
	args = parser.parse_args()
	print("args.port={}".format(args.port))

	print("myIp={}".format(myIp))
	myIpList = myIp.split('.')
	print("myIpList={}".format(myIpList))

	#address = "192.168.10.255" # for Broadcast
	address = "{}.{}.{}.255".format(myIpList[0], myIpList[1], myIpList[2])
	print("address={}".format(address))

	client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	client.bind(('', args.port))
	client.sendto(b'take picture', (address, args.port))
	client.close()
