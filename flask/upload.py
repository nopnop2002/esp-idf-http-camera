# -*- coding: utf-8 -*-
import os
import werkzeug
from datetime import datetime
from flask import Flask, request, make_response, jsonify
from werkzeug.serving import WSGIRequestHandler

# flask
app = Flask(__name__)

# ex) set UPLOAD_DIR_PATH=C:/tmp/flaskUploadDir
UPLOAD_DIR = os.getenv("UPLOAD_DIR_PATH")
print("UPLOAD_DIR={}".format(UPLOAD_DIR))
if (UPLOAD_DIR == None):
    UPLOAD_DIR = os.getcwd()
    print("UPLOAD_DIR={}".format(UPLOAD_DIR))

@app.route("/")
def hello():
    print("hello")
    return "Hello World!"

# Receive Filename, Type and Data as multipart / form-data.
# request.files with multipart/form-data
#
# <form action="/upload_multipart" method="post" enctype="multipart/form-data">
#   <input type="file" name="uploadFile"/>
#   <input type="submit" value="submit"/>
# </form>
#
@app.route('/upload_multipart', methods=['POST'])
def upload_multipart():
    print("upload_multipart start")
    print("request={}".format(request))
    print("request.files={}".format(request.files))

    if 'uploadFile' not in request.files:
        return make_response(jsonify({'result':'uploadFile is required'}))

    file = request.files['uploadFile']
    '''
    file is werkzeug.datastructures.FileStorage Object.
    This object have these member.
        filename：Uploaded File Name
        name：Field name of Form
        headers：HTTP request header information(header object of flask)
        content_length：content-length of HTTP request
        mimetype：mimetype

    '''
    print("type(file)={}".format(type(file)))
    fileName = file.filename
    contentType = file.mimetype
    print("fileName={} contentType={}".format(fileName, contentType))
    if '' == fileName:
        return make_response(jsonify({'result':'filename must not empty.'}))

    saveFileName = datetime.now().strftime("%Y%m%d_%H%M%S_") \
        + werkzeug.utils.secure_filename(fileName)
    print("saveFileName={}".format(saveFileName))
    file.save(os.path.join(UPLOAD_DIR, saveFileName))
    return "{'result':'upload OK'}"

# main
if __name__ == "__main__":
    WSGIRequestHandler.protocol_version = "HTTP/1.1"
    print("app.url_map={}".format(app.url_map))
    app.run(host='0.0.0.0', port=8080, debug=True)
