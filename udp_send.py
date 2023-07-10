#!/usr/bin/python
#-*- encoding: utf-8 -*-
#
# Requirement library
# python3 -m pip install -U netifaces

from socket import *
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

print("myIp={}".format(myIp))
myIpList = myIp.split('.')
print("myIpList={}".format(myIpList))

HOST = ''
PORT = 49876
#ADDRESS = "192.168.10.255" # for Broadcast
ADDRESS = "{}.{}.{}.255".format(myIpList[0], myIpList[1], myIpList[2])
print("ADDRESS={}".format(ADDRESS))

s = socket(AF_INET, SOCK_DGRAM)
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
s.bind((HOST, PORT))

s.sendto(b'take picture', (ADDRESS, PORT))

s.close()
