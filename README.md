# esp-idf-http-camera
Take a picture and Publish it via HTTP.   
This project use [ESP32 Camera Driver](https://github.com/espressif/esp32-camera).

![slide-0001](https://user-images.githubusercontent.com/6020549/119491922-7a092e00-bd99-11eb-8260-a52e9f5bddc2.jpg)
![slide-0002](https://user-images.githubusercontent.com/6020549/119491927-7bd2f180-bd99-11eb-88aa-a4c4c9ab6c84.jpg)

# Server Side
Download the server from [here](https://github.com/nopnop2002/multipart-upload-server).

![http-server](https://user-images.githubusercontent.com/6020549/119244044-79a94100-bba7-11eb-8b03-e25e78fc310a.jpg)
![http-server-2](https://user-images.githubusercontent.com/6020549/119245826-2808b280-bbb7-11eb-81f1-87bf00f88e41.jpg)
![http-server-3](https://user-images.githubusercontent.com/6020549/119248815-a886dd80-bbce-11eb-8974-68f26d08ed3b.jpg)

---

# ESP32 Side

## Software requirements
esp-idf v4.3 or later.   

## Installation
Use a USB-TTL converter.   

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
git clone https://github.com/espressif/esp32-camera components/esp32-camera
idf.py set-target esp32
idf.py menuconfig
idf.py flash monitor
```

## Start firmware
Change GPIO0 to open and press the RESET button.

## Configuration
Set the following items using menuconfig.

![config-main](https://user-images.githubusercontent.com/6020549/66692052-c17e9b80-ecd5-11e9-8316-075350ceb2e9.jpg)
![config-app](https://user-images.githubusercontent.com/6020549/119243496-50d27d00-bba2-11eb-9c75-4e06d4d19444.jpg)

### Wifi Setting

![config-wifi-1](https://user-images.githubusercontent.com/6020549/119243503-529c4080-bba2-11eb-92c5-b59f66f9fea6.jpg)

You can use the mDNS hostname instead of the IP address.   
- esp-idf V4.3 or earlier   
 You will need to manually change the mDNS strict mode according to [this](https://github.com/espressif/esp-idf/issues/6190) instruction.   
- esp-idf V4.4 or later  
 If you set CONFIG_MDNS_STRICT_MODE = y in sdkconfig.default, the firmware will be built with MDNS_STRICT_MODE = 1.

![config-wifi-2](https://user-images.githubusercontent.com/6020549/119243504-5334d700-bba2-11eb-8c77-f958251d8611.jpg)

You can use static IP.   
![config-wifi-3](https://user-images.githubusercontent.com/6020549/119243505-5334d700-bba2-11eb-9677-47cb6d1f9536.jpg)



### HTTP Server Setting

![config-http](https://user-images.githubusercontent.com/6020549/99767832-129ec700-2b47-11eb-9bc7-f483d20b8c5e.jpg)

### Attached File Name Setting

You can choose the file name on the HTTP server side from the following.   
- Always the same file name   
- File name based on date and time   
When you choose date and time file name, you will need an NTP server.   
The file name will be YYYYMMDD-hhmmss.jpg.   

![config-filename-1](https://user-images.githubusercontent.com/6020549/119243498-5203aa00-bba2-11eb-87d5-053636dbb85a.jpg)
![config-filename-2](https://user-images.githubusercontent.com/6020549/119243499-5203aa00-bba2-11eb-8c0f-6bb42d125d64.jpg)

- Add FrameSize to Remote file Name   
When this is enabled, FrameSize is added to remote file name like this.   
`20210520-165740_800x600.jpg`   

![config-filename-3](https://user-images.githubusercontent.com/6020549/119243501-529c4080-bba2-11eb-8ba4-85cdd764b0fc.jpg)

### Select Frame Size
Large frame sizes take longer to take a picture.   
![config-framesize-1](https://user-images.githubusercontent.com/6020549/118947689-8bfe6180-b992-11eb-8657-b4e86d3acc70.jpg)
![config-framesize-2](https://user-images.githubusercontent.com/6020549/118947692-8d2f8e80-b992-11eb-9caa-1f6b6cb2210e.jpg)

### Select Shutter

You can choose one of the following shutter methods

- Shutter is the Enter key on the keyboard   
For operation check

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
You can use tcp_send.py.   
`python ./tcp_send.py`

![config-shutter-3](https://user-images.githubusercontent.com/6020549/99890070-dc745b00-2c9e-11eb-9ae8-45ac11db5db5.jpg)

- Shutter is UDP Socket   
You can use udp_send.py.   
`python ./udp_send.py`

![config-shutter-4](https://user-images.githubusercontent.com/6020549/99889941-658a9280-2c9d-11eb-8bc7-06f2b67af3cb.jpg)

- Shutter is HTTP Request   
You can use this command.   
`curl "http://esp32-camera.local:8080/take_picture"`

![config-shutter-5](https://user-images.githubusercontent.com/6020549/99889881-b6e65200-2c9c-11eb-96c2-6fdde929dbe0.jpg)


### Flash Light

ESP32-CAM by AI-Thinker have flash light on GPIO4.

![config-flash](https://user-images.githubusercontent.com/6020549/99890190-0b3f0100-2ca0-11eb-94c6-ba7e2cfe1727.jpg)


