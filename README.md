# esp-idf-http-camera
Take a picture and Publish it via HTTP.   
This project use [this](https://components.espressif.com/components/espressif/esp32-camera) Camera Driver.   

![slide-0001](https://user-images.githubusercontent.com/6020549/119491922-7a092e00-bd99-11eb-8260-a52e9f5bddc2.jpg)
![slide-0002](https://user-images.githubusercontent.com/6020549/119491927-7bd2f180-bd99-11eb-88aa-a4c4c9ab6c84.jpg)

# HTTP Server
You can use a server using tornado/flask or a simple server.   

- tornado/flask server   
	Download the tornado/flask server from [here](https://github.com/nopnop2002/multipart-upload-server).
	![http-server](https://user-images.githubusercontent.com/6020549/119244044-79a94100-bba7-11eb-8b03-e25e78fc310a.jpg)
	![http-server-2](https://user-images.githubusercontent.com/6020549/119245826-2808b280-bbb7-11eb-81f1-87bf00f88e41.jpg)
	![http-server-3](https://user-images.githubusercontent.com/6020549/119248815-a886dd80-bbce-11eb-8974-68f26d08ed3b.jpg)

- simple server
	```
	python3 -m pip install -U wheel
	python3 -m pip install opencv-python

	python3 ./server.py --help
	usage: server.py [-h] [--port PORT]

	options:
	  -h, --help   show this help message and exit
	  --port PORT  http port
	```

	__Close the image window with the ESC key.__   
	![opencv](https://github.com/nopnop2002/esp-idf-mqtt-camera/assets/6020549/516b2f25-d285-47d6-ae56-ee1cceed5c58)

---

# ESP32 Side

# Hardware requirements
ESP32 development board with OV2640 camera.   
If you use other camera, edit sdkconfig.default.   
From the left:   
- Aithinker ESP32-CAM   
- Freenove ESP32-WROVER CAM   
- UICPAL ESPS3 CAM   
- Freenove ESP32S3-WROVER CAM (Clone)   

![es32-camera](https://github.com/nopnop2002/esp-idf-websocket-camera/assets/6020549/38dbef9a-ed85-4df2-8d22-499b2b497278)

## Software requirements
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   


## Installation
For AiThinker ESP32-CAM, you need to use a USB-TTL converter and connect GPIO0 to GND.   

|ESP-32|USB-TTL|
|:-:|:-:|
|U0TXD|RXD|
|U0RXD|TXD|
|GPIO0|GND|
|5V|5V|
|GND|GND|


```
git clone https://github.com/nopnop2002/esp-idf-http-camera
cd esp-idf-http-camera
idf.py set-target {esp32/esp32s3}
idf.py menuconfig
idf.py flash monitor
```

## Start firmware
For AiThinker ESP32-CAM, Change GPIO0 to open and press the RESET button.

## Configuration
Set the following items using menuconfig.

![config-main](https://github.com/user-attachments/assets/3bdb6876-d47c-4396-88d1-395ffaeda2cc)
![config-app](https://user-images.githubusercontent.com/6020549/200204471-35b658fc-40b7-47aa-b0ba-86979049eba4.jpg)

### Wifi Setting

![config-wifi-1](https://user-images.githubusercontent.com/6020549/119243503-529c4080-bba2-11eb-92c5-b59f66f9fea6.jpg)

You can connect using the mDNS hostname instead of the IP address.   
![config-wifi-2](https://user-images.githubusercontent.com/6020549/119243504-5334d700-bba2-11eb-8c77-f958251d8611.jpg)

You can use static IP.   
![config-wifi-3](https://user-images.githubusercontent.com/6020549/119243505-5334d700-bba2-11eb-9677-47cb6d1f9536.jpg)


### HTTP Server Setting

![config-http](https://user-images.githubusercontent.com/6020549/183002951-42e79610-eccf-4c8b-a03f-b6c5a9679bc2.jpg)

### Attached File Name Setting

You can choose the file name on the HTTP server side from the following.   
- Always the same file name   
	![config-filename-1](https://user-images.githubusercontent.com/6020549/119243498-5203aa00-bba2-11eb-87d5-053636dbb85a.jpg)
- File name based on date and time   
	When you choose date and time file name, you will need an NTP server.   
	The file name will be YYYYMMDD-hhmmss.jpg.   
	![config-filename-2](https://user-images.githubusercontent.com/6020549/119243499-5203aa00-bba2-11eb-8c0f-6bb42d125d64.jpg)

- Add FrameSize to Remote file Name   
	When this is enabled, FrameSize is added to remote file name like this.   
	`20210520-165740_800x600.jpg`   
	![config-filename-3](https://user-images.githubusercontent.com/6020549/119243501-529c4080-bba2-11eb-8ba4-85cdd764b0fc.jpg)


### Select Board
![config-board](https://github.com/nopnop2002/esp-idf-http-camera/assets/6020549/732c9574-68df-4e52-9b9e-6071d7ce5737)


### Select Frame Size
Large frame sizes take longer to take a picture.   
![config-framesize-1](https://user-images.githubusercontent.com/6020549/118947689-8bfe6180-b992-11eb-8657-b4e86d3acc70.jpg)
![config-framesize-2](https://user-images.githubusercontent.com/6020549/118947692-8d2f8e80-b992-11eb-9caa-1f6b6cb2210e.jpg)

### Select Shutter

You can choose one of the following shutter methods

- Shutter is the Enter key on the keyboard   
	For operation check.   
	When using the USB port provided by the USB Serial/JTAG Controller Console, you need to enable the following line in sdkconfig.
	```
	CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
	```
	![config-shutter-1](https://user-images.githubusercontent.com/6020549/99890068-db432e00-2c9e-11eb-84e2-4e6c5f05fb7a.jpg)


- Shutter is a GPIO toggle   

	- Initial Sate is PULLDOWN   
	The shutter is prepared when it is turned from OFF to ON, and a picture is taken when it is turned from ON to OFF.   

	- Initial Sate is PULLUP   
	The shutter is prepared when it is turned from ON to OFF, and a picture is taken when it is turned from OFF to ON.   

	I confirmed that the following GPIO can be used.   

	|GPIO|PullDown|PullUp|
	|:-:|:-:|:-:|
	|GPIO12|OK|NG|
	|GPIO13|OK|OK|
	|GPIO14|OK|OK|
	|GPIO15|OK|OK|
	|GPIO16|NG|NG|

	![config-shutter-2](https://user-images.githubusercontent.com/6020549/99897437-d2714d00-2cdc-11eb-8e59-c8bf4ef25d62.jpg)

- Shutter is TCP Socket   
	ESP32 acts as a TCP server and listens for requests from TCP clients.   
	You can use tcp_send.py as shutter.   
	`python3 ./tcp_send.py`
	![Image](https://github.com/user-attachments/assets/4c301018-2f8c-4644-be3f-417222fb1842)

- Shutter is UDP Socket   
	ESP32 acts as a UDP listener and listens for requests from UDP clients.   
	You can use this command as shutter.   
	`echo -n "take" | socat - UDP-DATAGRAM:255.255.255.255:49876,broadcast`   
	You can use udp_send.py as shutter.   
	Requires netifaces.   
	`python3 ./udp_send.py`   
	![Image](https://github.com/user-attachments/assets/3dcd72be-d0ef-4bd9-9273-f420ca88f11b)   
	You can use these devices as shutters.   
	![Image](https://github.com/user-attachments/assets/cc97da4e-6c06-4604-8362-f81c6fb6eb58)   
	Click [here](https://github.com/nopnop2002/esp-idf-selfie-trigger) for details.   

- Shutter is HTTP Request   
	ESP32 acts as a HTTP server and listens for requests from HTTP clients.   
	You can use this command as shutter.   
	`curl "http://esp32-camera.local:8080/take/picture"`
	![config-shutter-5](https://user-images.githubusercontent.com/6020549/193444800-ed7ac318-307d-4c12-baec-9b32b98df77c.jpg)


### Flash Light   
ESP32-CAM by AI-Thinker have flash light on GPIO4.

![config-flash](https://user-images.githubusercontent.com/6020549/99890190-0b3f0100-2ca0-11eb-94c6-ba7e2cfe1727.jpg)

## PSRAM   
When you use ESP32S3-WROVER CAM, you need to set the PSRAM type.   

![config-psram](https://github.com/nopnop2002/esp-idf-websocket-camera/assets/6020549/ba04f088-c628-46ac-bc5b-2968032753e0)

# View picture using Built-in WEB Server
ESP32 works as a web server.   
You can view the pictures taken using the built-in WEB server.   
Enter the ESP32's IP address and port number in the address bar of your browser.   
You can connect using mDNS hostname instead of IP address.   

![browser](https://user-images.githubusercontent.com/6020549/124227364-837a7880-db45-11eb-9d8b-fa15c676adac.jpg)
