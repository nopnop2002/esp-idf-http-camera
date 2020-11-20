# -*- coding: utf-8 -*-
import os
import werkzeug
from datetime import datetime
import tornado.httpserver
import tornado.ioloop
import tornado.options
import tornado.web

from tornado.options import define, options
define("port", default=8080, help="run on the given port", type=int)

# ex) set UPLOAD_DIR_PATH=C:/tmp/flaskUploadDir
UPLOAD_DIR = os.getenv("UPLOAD_DIR_PATH")
print("UPLOAD_DIR={}".format(UPLOAD_DIR))
if (UPLOAD_DIR == None):
    UPLOAD_DIR = os.getcwd()
    print("UPLOAD_DIR={}".format(UPLOAD_DIR))

class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        print("IndexHandler:get")
        self.write("Hello World")

class UploadHandler(tornado.web.RequestHandler):
    def post(self):
        print("UploadHandler:post")
        if 'uploadFile' not in self.request.files:
            responce = {"result": "upload FAIL.uploadFile is not included in the request."}
            self.write(json.dumps(responce))
            return

        files = self.request.files['uploadFile']
        #print("type(files)={}".format(type(files)))
        #print("files={}".format(files))
        file = files[0]
        fileName = file['filename']
        fileBody = file['body']
        print("fileName={}".format(fileName))

        saveFileName = datetime.now().strftime("%Y%m%d_%H%M%S_") \
            + werkzeug.utils.secure_filename(fileName)
        print("saveFileName={}".format(saveFileName))

        f = open(saveFileName, "wb")
        f.write(fileBody)
        f.close()
        responce = {"result": "upload OK"}
        self.write(responce)

def make_app():
    return tornado.web.Application([
        (r"/", IndexHandler),
        (r"/upload_multipart", UploadHandler),
    ],debug=True)

if __name__ == "__main__":
    tornado.options.parse_command_line()
    app = make_app()
    #app = tornado.web.Application(handlers=[(r"/", IndexHandler)],debug=True)
    http_server = tornado.httpserver.HTTPServer(app)
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.current().start()


