#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import time
import json
import subprocess
import pathlib
import datetime

# Non standard library
# python3 -m pip install -U pillow
from PIL import Image
# python3 -m pip install -U python-magic
import magic
# python3 -m pip install -U requests
import requests
# python3 -m pip install -U flask
#from flask import Flask, render_template, request, redirect, url_for
from flask import *
# Override flask.logging
import logging
# python3 -m pip install -U Werkzeug
import werkzeug
#from werkzeug.serving import WSGIRequestHandler
# python3 -m pip install piexif
import piexif

app = Flask(__name__)
logging.basicConfig(level=logging.INFO)

# Initialize node list
nodeList = []
waitList = []

# create UPLOAD_DIR
UPLOAD_DIR = os.path.join(os.getcwd(), "uploaded")
logging.info("UPLOAD_DIR={}".format(UPLOAD_DIR))
if (os.path.exists(UPLOAD_DIR) == False):
	logging.warning("UPLOAD_DIR [{}] not found. Create this".format(UPLOAD_DIR))
	os.mkdir(UPLOAD_DIR)

# Added /uploaded to static_url_path
add_app = Blueprint("uploaded", __name__, static_url_path='/uploaded', static_folder='./uploaded')
app.register_blueprint(add_app)

# Execute external process
def exec_subprocess(cmd: str, raise_error=True):
	child = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout, stderr = child.communicate()
	rt = child.returncode
	if rt != 0 and raise_error:
		raise Exception(f"command return code is not 0. got {rt}. stderr = {stderr}")

	return stdout, stderr, rt

@app.route("/")
def get():
	fileList = []
	dirList = []
	for (dirpath, dirnames, filenames) in os.walk(UPLOAD_DIR):
		logging.debug("filenames={}".format(filenames))
		for name in filenames:
			nm = os.path.join(dirpath, name).replace(UPLOAD_DIR, "").strip("/").split("/")
			logging.debug("nm={}".format(nm))
			# Skip if the file is in a subdirect
			# nm=['templates', 'index.html']
			if len(nm) != 1: continue

			fullpath = os.path.join(dirpath, name)
			logging.debug("fullpath={}".format(fullpath))
			if os.path.isfile(fullpath) == False: continue
			size = os.stat(fullpath).st_size

			mime = magic.from_file(fullpath, mime=True)
			logging.debug("mime={}".format(mime))
			visible = "image/" in mime
			if (visible == False):
				visible = "text/" in mime
			logging.debug("visible={}".format(visible))

			p = pathlib.Path(fullpath)
			dt = datetime.datetime.fromtimestamp(p.stat().st_ctime)
			ctime = dt.strftime('%Y/%m/%d %H:%M:%S') 

			exif = ""
			if (mime == "image/jpeg"):
				image = Image.open(fullpath)
				try:
					exif_dict = piexif.load(image.info["exif"])
					exif = exif_dict["Exif"][piexif.ExifIFD.UserComment].decode("utf-8")
					exif = exif.replace('ASCII', '')
				except:
					pass
				logging.debug("exif=[{}]".format(exif))

			fileList.append({
				"name": name,
				"size": str(size) + " B",
				"mime": mime,
				"fullname": fullpath,
				"visible": visible,
				"exif": exif,
				"ctime": ctime,
			})

	# Handling missing nodes
	nowtime = time.time()
	for i in range(len(nodeList)):
		logging.debug("nodeList[{}]={}".format(i, nodeList[i]))
		timestamp = nodeList[i][6]
		difftime = nowtime - timestamp
		logging.debug("timestamp={} difftime={}".format(timestamp, difftime))
		if (difftime > 30):
			del nodeList[i]
			break;

	for i in range(len(waitList)):
		logging.debug("waitList[{}]={}".format(i, waitList[i]))
		timestamp = waitList[i][2]
		difftime = nowtime - timestamp
		logging.debug("timestamp={} difftime={}".format(timestamp, difftime))
		if (difftime > 30):
			del waitList[i]
			break;

	meta = {
		"current_directory": UPLOAD_DIR,
		"node_list_count": len(nodeList),
	}

	logging.info("nodeList={}".format(nodeList))
	logging.info("waitList={}".format(waitList))
	logging.debug("dirList={}".format(dirList))
	logging.debug("files={}".format(fileList))
	logging.debug("meta={}".format(meta))
	templateData = {
		'nodes' : sorted(nodeList),
		'files' : sorted(fileList, key=lambda k: k["name"].lower()),
		'folders': dirList,
		'meta' : meta
	}
	return render_template("index.html", **templateData)

@app.route("/download")
def download():
	filename = request.args.get('filename', default=None, type=str)
	logging.info("{}:filename={}".format(sys._getframe().f_code.co_name, filename))

	if os.path.isfile(filename):
		if os.path.dirname(filename) == UPLOAD_DIR.rstrip("/"):
			return send_file(filename, as_attachment=True)
		else:
			return render_template("no_permission.html")
	else:
		return render_template("not_found.html")
	return None

@app.route("/imageview")
def imageview():
	filename = request.args.get('filename', default=None, type=str)
	logging.info("{}:filename={}".format(sys._getframe().f_code.co_name, filename))
	rotate = request.args.get('rotate', default=0, type=int)
	logging.info("{}:rotate={}{}".format(sys._getframe().f_code.co_name, rotate, type(rotate)))

	mime = magic.from_file(filename, mime=True)
	mime = mime.split("/")
	logging.info("{}:mime={}".format(sys._getframe().f_code.co_name, mime))

	if (mime[0] == "image"):
		logging.debug("filename={}".format(filename))
		filename = os.path.basename(filename)
		logging.debug("filename={}".format(filename))
		filename = os.path.join("/uploaded", filename)
		logging.debug("filename={}".format(filename))
		return render_template("view.html", user_image = filename, rotate=rotate)

	if (mime[0] == "text"):
		contents = ""
		f = open(filename, 'r')
		datalist = f.readlines()
		for data in datalist:
			logging.debug("[{}]".format(data.rstrip()))
			contents = contents + data.rstrip() + "<br>"
		return contents

@app.route('/upload_multipart', methods=['POST'])
def upload_multipart():
	logging.info("upload_multipart")
	logging.info("request={}".format(request))
	logging.info("request.files={}".format(request.files))
	logging.info("request.remote_addr={}".format(request.remote_addr))
	dict = request.files.to_dict(flat=False)
	logging.info("dict={}".format(dict))


	'''
	file is werkzeug.datastructures.FileStorage Object.
	This object have these member.
		filename：Uploaded File Name
		name：Field name of Form
		headers：HTTP request header information(header object of flask)
		content_length：content-length of HTTP request
		mimetype：mimetype

	'''

	FileStorage = dict['upfile'][0]
	logging.info("FileStorage={}".format(FileStorage))
	logging.info("FileStorage.filename={}".format(FileStorage.filename))
	logging.info("FileStorage.mimetype={}".format(FileStorage.mimetype))

	filename = FileStorage.filename
	filepath = os.path.join(UPLOAD_DIR, werkzeug.utils.secure_filename(filename))
	#FileStorage.save(filepath)

	try:
		FileStorage.save(filepath)
		responce = {'result':'upload OK'}
		logging.info("{} uploaded {}, saved as {}".format(request.remote_addr, filename, filepath))

		# Add metadata to jpeg
		logging.info("request.remote_addr={}".format(request.remote_addr))
		for i in range(len(waitList)):
			wait_addr = waitList[i][0]
			wait_exif = waitList[i][1]
			logging.info("wait_addr={} wait_exif=[{}]".format(wait_addr, wait_exif))
			if (wait_addr == request.remote_addr):
				if (wait_exif != ""):
					# Add exif to usercomment using exiftool
					cmd = "exiftool -usercomment='{}' {}".format(wait_exif, filepath)
					logging.info("cmd=[{}]".format(cmd))
					stdout, stderr, rt = exec_subprocess(cmd)
					logging.info("exec_subprocess stdout={} stderr={} rt={}".format(stdout, stderr, rt))
					backpath = filepath + "_original"
					os.remove(backpath)
				del waitList[i]
				break;

	except IOError as e:
		logging.error("Failed to write file due to IOError %s", str(e))
		responce = {'result':'upload FAIL'}

	#return json.dumps(responce)
	return Response(json.dumps(responce), mimetype='text/plain')

@app.route("/select", methods=["POST"])
def select():
	logging.info("select: request.form={}".format(request.form))
	selected = request.form.getlist('selected')
	logging.info("selected={}".format(selected))
	logging.info("len(selected)={}".format(len(selected)))
	for ip in selected:
		exif = request.form.get('exif')
		logging.info("select: exif=[{}]".format(exif))
		logging.info("select: ip=[{}]".format(ip))
		url = "http://{}:8080/post".format(ip)
		logging.info("select: url={}".format(url))
		Uresponse = requests.post(url)
		logging.info("select: Uresponse={}".format(Uresponse))

		# Add to waiting list for upload for metadata
		utime = time.time()
		wait = []
		wait.append(ip) #0
		wait.append(exif) #1
		wait.append(utime) #2
		waitList.append(wait)
		
	return redirect(url_for('get'))

@app.route("/node_information", methods=["POST"])
def post():
	logging.debug("post: request={}".format(request))
	logging.debug("post: request.data={}".format(request.data))
	payload = request.data
	if (type(payload) is bytes):
		payload = payload.decode('utf-8')
	logging.info("post: payload={}".format(payload))
	json_object = json.loads(payload)
	mac = json_object['mac']
	logging.debug("post: mac=[{}]".format(mac))
	logging.debug("post: nodeList={}".format(nodeList))
	nodeIndex = None
	for i in range(len(nodeList)):
		logging.debug("post: nodeList[{}]={}".format(i, nodeList[i]))
		if (mac == nodeList[i][1]):
			nodeIndex = i

	logging.debug("post: nodeIndex={}".format(nodeIndex))
	node = []
	ip = json_object['ip']
	board = json_object['board']
	frame = json_object['frame']
	dummy1 = 0
	dummy2 = 0
	utime = time.time()
	node.append(ip) #0
	node.append(mac) #1
	node.append(board) #2
	node.append(frame) #3
	node.append(dummy1) #4
	node.append(dummy2) #5
	node.append(utime) #6
	if (nodeIndex is None):
		nodeList.append(node)
	else:
		nodeList[nodeIndex] = node

	responce = {'result':'post OK'}
	#return json.dumps(responce)
	return Response(json.dumps(responce), mimetype='text/plain')

if __name__ == "__main__":
	#WSGIRequestHandler.protocol_version = "HTTP/1.1"
	app.run(host='0.0.0.0', port=8080, debug=True)

