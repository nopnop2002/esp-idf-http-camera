# esp-idf-http-camera
Take a picture and Publish it via HTTP.   
This project use [this](https://components.espressif.com/components/espressif/esp32-camera) Camera Driver.   

![slide-0001](https://user-images.githubusercontent.com/6020549/119491922-7a092e00-bd99-11eb-8260-a52e9f5bddc2.jpg)
![slide-0002](https://user-images.githubusercontent.com/6020549/119491927-7bd2f180-bd99-11eb-88aa-a4c4c9ab6c84.jpg)

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

# Start HTTP Server
You can use a server using flask or a simple server.   

## flask HTTP server   
```
sudo apt update
sudo apt install python3-pip python3-setuptools libimage-exiftool-perl jhead
python3 -m pip install -U pip
python3 -m pip install -U wheel
python3 -m pip install -U Werkzeug
python3 -m pip install -U pillow
python3 -m pip install -U python-magic
python3 -m pip install -U requests
python3 -m pip install -U flask
python3 -m pip install -U piexif
git clone https://github.com/nopnop2002/esp-idf-http-camera
cd esp-idf-http-camera/flask
python3 main.py
```

Open your browser and enter the host address in the address bar.   
![Image](https://github.com/user-attachments/assets/01427bcf-e51d-40cd-a6cf-03fa41a2912f)

When you start ESP32, a list of ESP32 will be displayed.   
![Image](https://github.com/user-attachments/assets/4b42727e-5afc-4d95-bbec-d57b9131ca45)

Select ESP32 and then press the Take Picture button.   
You can add Exif to JPEG.   
![Image](https://github.com/user-attachments/assets/62d4a56e-226b-4131-8d96-de8185e5441c)

ESP32 takes a photo and transmits it to the server.   
You can see the photos.   
![Image](https://github.com/user-attachments/assets/885a06e4-50d0-42dd-a475-29ec8b24a85d)
![Image](https://github.com/user-attachments/assets/5d7207c9-a4f0-448e-9e76-5c16dd87b8be)
![Image](https://github.com/user-attachments/assets/898c799f-4c6a-4416-9452-2d0fab5ec94a)
![Image](https://github.com/user-attachments/assets/fae46e75-7f57-4807-b425-cfd206162a81)

- About Exif tags   
	In this project, we will use exiftool to add Exif tags to JPG files.   
	The name of the tag to be added is `UserComment`.   
	You can view the Exif content with the following command:   
	`jhead *.jpg`   
	![Image](https://github.com/user-attachments/assets/7506b9b3-bbbf-414b-b5f3-2ee665fcf7d2)

	You can use exiftool to modify the Exif tags.   
	Exiftool will save the file with the original Exif tag changes under a different name.   
	```
	$ jhead picture2_800x600.jpg
	File name    : picture2_800x600.jpg
	File size    : 15020 bytes
	File date    : 2025:04:23 03:47:27
	Resolution   : 800 x 600
	JPEG Quality : 62
	Comment      : test-03

	$ exiftool -usercomment=test-03-01 picture2_800x600.jpg
	    1 image files updated

	$ jhead picture2_800x600.jpg
	File name    : picture2_800x600.jpg
	File size    : 15022 bytes
	File date    : 2025:04:23 04:11:26
	Resolution   : 800 x 600
	JPEG Quality : 62
	Comment      : test-03-01

	$ ls picture2_800x600*
	picture2_800x600.jpg  picture2_800x600.jpg_original
	```

	jhead treats UserComment tags the same as Comment tags.   
	exiftool distinguishes between UserComment and Comment tags.   
	An explanation of the difference between UserComment tags and Comment tags can be found [here](https://exiftool.org/forum/index.php?topic=12466.0).   


## Simple HTTP server
```
python3 -m pip install -U wheel
python3 -m pip install opencv-python
git clone https://github.com/nopnop2002/esp-idf-http-camera
python3 ./http_server.py --help
usage: http_server.py [-h] [--port PORT] [--timeout TIMEOUT]

options:
  -h, --help         show this help message and exit
  --port PORT        http port
  --timeout TIMEOUT  wait time for keyboard input[sec]
```
When timeout is specified, display the image for the specified number of seconds.   
When timeout is not specified, the image will be displayed until the ESC key is pressed.   
New requests are queued while the image is displayed.   
__Close the image window with the ESC key. Do not use the close button.__   
![opencv](https://github.com/nopnop2002/esp-idf-mqtt-camera/assets/6020549/516b2f25-d285-47d6-ae56-ee1cceed5c58)   
This script works not only on Linux but also on Windows 10.   
I used Python 3.9.13 for Windows.   
![Image](https://github.com/user-attachments/assets/b1d4f037-3be3-4b02-bea7-6856aa2e1f8e)


## Installation for ESP32
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
![config-main](https://github.com/user-attachments/assets/3bdb6876-d47c-4396-88d1-395ffaeda2cc)
![config-app](https://user-images.githubusercontent.com/6020549/200204471-35b658fc-40b7-47aa-b0ba-86979049eba4.jpg)

### Wifi Setting
Set the information of your access point.   
![config-wifi-1](https://user-images.githubusercontent.com/6020549/119243503-529c4080-bba2-11eb-92c5-b59f66f9fea6.jpg)   
You can connect using the mDNS hostname instead of the IP address.   
![config-wifi-2](https://user-images.githubusercontent.com/6020549/119243504-5334d700-bba2-11eb-8c77-f958251d8611.jpg)   
You can use static IP.   
![config-wifi-3](https://user-images.githubusercontent.com/6020549/119243505-5334d700-bba2-11eb-9677-47cb6d1f9536.jpg)   


### HTTP Server Setting
Specify the IP address and port number of the http server.   
You can use mDNS hostnames instead of IP addresses.   
![Image](https://github.com/user-attachments/assets/1d8ddb66-9442-4dac-a214-87ea6b2c4ca6)

### Attached File Name Setting
You can select the file name to send to the HTTP server from the following.   
- Always the same file name   
	![config-filename-1](https://user-images.githubusercontent.com/6020549/119243498-5203aa00-bba2-11eb-87d5-053636dbb85a.jpg)
- File name based on date and time   
	When you choose date and time file name, you will need an NTP server.   
	The file name will be YYYYMMDD-hhmmss.jpg.   
	![config-filename-2](https://user-images.githubusercontent.com/6020549/119243499-5203aa00-bba2-11eb-8c0f-6bb42d125d64.jpg)

- Add FrameSize to Remote file Name   
	When this is enabled, FrameSize is added to remote file name like this.   
	`picture_800x600.jpg`   
	`20210520-165740_800x600.jpg`   
	![config-filename-3](https://user-images.githubusercontent.com/6020549/119243501-529c4080-bba2-11eb-8ba4-85cdd764b0fc.jpg)


### Select Board
![config-board](https://github.com/nopnop2002/esp-idf-http-camera/assets/6020549/732c9574-68df-4e52-9b9e-6071d7ce5737)


### Select Frame Size
Large frame sizes take longer to take a picture.   
![config-framesize-1](https://user-images.githubusercontent.com/6020549/118947689-8bfe6180-b992-11eb-8657-b4e86d3acc70.jpg)
![config-framesize-2](https://user-images.githubusercontent.com/6020549/118947692-8d2f8e80-b992-11eb-9caa-1f6b6cb2210e.jpg)

### Select Shutter
ESP32 acts as a HTTP server and listens for requests from HTTP clients.   
You can use this command as shutter.   
`curl -X POST http://{ESP32's IP Address}:8080/post`   
If your ESP32's IP address is `192.168.10.157`, it will look like this.   
`curl -X POST http://192.168.10.157:8080/post`   

In addition to this, you can select the following triggers:   

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


### Flash Light   
ESP32-CAM by AI-Thinker have flash light on GPIO4.

![config-flash](https://user-images.githubusercontent.com/6020549/99890190-0b3f0100-2ca0-11eb-94c6-ba7e2cfe1727.jpg)

## PSRAM   
When you use ESP32S3-WROVER CAM, you need to change the PSRAM type.   

![config-psram](https://github.com/nopnop2002/esp-idf-websocket-camera/assets/6020549/ba04f088-c628-46ac-bc5b-2968032753e0)

# View picture using Built-in WEB Server
ESP32 works as a web server.   
You can view the pictures taken using the built-in WEB server.   
Enter the ESP32's IP address and port number in the address bar of your browser.   
You can connect using mDNS hostname instead of IP address.   

![browser](https://user-images.githubusercontent.com/6020549/124227364-837a7880-db45-11eb-9d8b-fa15c676adac.jpg)
