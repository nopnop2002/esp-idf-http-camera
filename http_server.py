#!/usr/bin/env python
# -*- coding: utf-8 -*-
# python3 -m pip install -U wheel
# python3 -m pip install opencv-python
import queue
import threading
import time
import http.server as server
import cv2
import argparse

queue01 = queue.Queue()
queue02 = queue.Queue()
imageFileName = "output.jpg"

def threadView(q1, q2):
	time.sleep(1)
	localVal = threading.local()
	while True:
		if q1.empty():
			time.sleep(1.0)
			#print("thread waiting..")
		else:
			localVal = q1.get()
			print("thread q1.get() localVal={} timeout={}".format(localVal, args.timeout))
			image = cv2.imread(imageFileName)
			cv2.imshow('image', image)
			#cv2.waitKey(0)
			cv2.waitKey(args.timeout*1000)
			cv2.destroyWindow('image')
			print("thread q2.put() localVal={}".format(localVal))
			q2.put(localVal)


class MyHandler(server.BaseHTTPRequestHandler):
	def do_POST(self):
		self.send_response(200)
		self.send_header('Content-Type', 'text/plain; charset=utf-8')
		self.end_headers()
		self.wfile.write(b'{"result": "post OK"}')

		print('path=[{}]'.format(self.path))
		if (self.path != "/upload_multipart"): return

		# Get request
		content_len  = int(self.headers.get("content-length"))
		#body = self.rfile.read(content_len).decode('utf-8')
		body = self.rfile.read(content_len)
		print("content_len={}".format(content_len))
		print("type(body)={}".format(type(body)))
		start = body.find(b"\r\n\r\n")
		print("start={}".format(start))
		end = body.find(b"\r\n--X-ESPIDF_MULTIPART--\r\n\r\n")
		print("end={}".format(end))

		image = body[start+4:end]
		#print("---------------------")
		#print(body)
		#print("---------------------")
		#print(image)

		outfile = open(imageFileName, 'wb')
		outfile.write(image)
		outfile.close

		queue01.put(0)
		while True:
			time.sleep(1)
			if queue02.empty():
				print("thread end waiting. ESC to end.")
				pass
			else:
				queue02.get()
				break
		print("thread end")

if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument('--port', type=int, help='http port', default=8080)
	parser.add_argument('--timeout', type=int, help='wait time for keyboard input[sec]', default=0)
	args = parser.parse_args() 
	print("args.port={}".format(args.port))
	print("args.timeout={}".format(args.timeout))

	thread = threading.Thread(target=threadView, args=(queue01,queue02,) ,daemon = True)
	thread.start()

	host = '0.0.0.0'
	httpd = server.HTTPServer((host, args.port), MyHandler)
	#httpd = server.ThreadingHTTPServer((host, port), MyHandler)
	httpd.serve_forever()
