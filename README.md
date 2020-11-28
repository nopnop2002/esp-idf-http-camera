# esp-idf-http-camera
Take a picture and Publish it via HTTP.   

![スライド1](https://user-images.githubusercontent.com/6020549/99890147-9c61a800-2c9f-11eb-8c2a-1f1ad9e1dd41.JPG)

![スライド2](https://user-images.githubusercontent.com/6020549/99767675-c81d4a80-2b46-11eb-9712-ec0be23d1685.JPG)

# Server Side
Download the server from [here](https://github.com/nopnop2002/multipart-upload-server).

---

# ESP32 Side

## Software requirements
esp-idf v4.0.2-120.   
git clone -b release/v4.0 --recursive https://github.com/espressif/esp-idf.git

esp-idf v4.1-520.   
git clone -b release/v4.1 --recursive https://github.com/espressif/esp-idf.git

esp-idf v4.2-beta1-227.   
git clone -b release/v4.2 --recursive https://github.com/espressif/esp-idf.git

__It does not work with esp-idf v4.3.__
__Even if I fix [this](https://github.com/espressif/esp-idf/pull/6029), I still get a panic.__

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
git clone https://github.com/espressif/esp32-camera components
make menuconfig
make flash monitor
```

## Start firmware
Change GPIO0 to open and press the RESET button.

## Configuration
Set the following items using menuconfig.

![config-main](https://user-images.githubusercontent.com/6020549/66692052-c17e9b80-ecd5-11e9-8316-075350ceb2e9.jpg)

![config-app](https://user-images.githubusercontent.com/6020549/99889856-738be380-2c9c-11eb-9ff5-871d424af66a.jpg)

### Wifi Setting

![config-wifi-1](https://user-images.githubusercontent.com/6020549/99889864-7f77a580-2c9c-11eb-98d2-4b229972d380.jpg)

You can use static IP.   
![config-wifi-2](https://user-images.githubusercontent.com/6020549/99889868-83a3c300-2c9c-11eb-9f3e-1b5650c34467.jpg)

### HTTP Server Setting

![config-http](https://user-images.githubusercontent.com/6020549/99767832-129ec700-2b47-11eb-9bc7-f483d20b8c5e.jpg)

### Attached File Name Setting

Select the attached file name from the following.   
- Always the same file name   
- File name based on date and time   
When you choose date and time file name, you will need an NTP server.   
The file name will be YYYYMMDD-hhmmss.jpg.   

![config-filename-1](https://user-images.githubusercontent.com/6020549/98748023-1d958100-23fc-11eb-8bc9-9b65306be2a3.jpg)
![config-filename-2](https://user-images.githubusercontent.com/6020549/98748025-1ec6ae00-23fc-11eb-9770-e00618b4097c.jpg)

### Camera Pin

![config-camerapin](https://user-images.githubusercontent.com/6020549/66692087-1d492480-ecd6-11e9-8b69-68191005a453.jpg)

### Picture Size

![config-picturesize](https://user-images.githubusercontent.com/6020549/66692095-26d28c80-ecd6-11e9-933e-ab0be911ecd2.jpg)

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

![config-shutter-2](https://user-images.githubusercontent.com/6020549/99890069-dbdbc480-2c9e-11eb-9ab5-2489efd0c579.jp

- Shutter is TCP Socket   
You can use tcp_send.py.   
```
python ./tcp_send.py
```

![config-shutter-3](https://user-images.githubusercontent.com/6020549/99890070-dc745b00-2c9e-11eb-9ae8-45ac11db5db5.jpg)

- Shutter is UDP Socket   
You can use udp_send.py.   
```
python ./udp_send.py
```

![config-shutter-4](https://user-images.githubusercontent.com/6020549/99889941-658a9280-2c9d-11eb-8bc7-06f2b67af3cb.jpg)

- Shutter is HTTP Request   
You can use this command.   

```
curl "http://192.168.10.110:8080/take_picture"
```

![config-shutter-5](https://user-images.githubusercontent.com/6020549/99889881-b6e65200-2c9c-11eb-96c2-6fdde929dbe0.jpg)


## Flash Light

ESP32-CAM by AI-Thinker have flash light on GPIO4.

![config-flash](https://user-images.githubusercontent.com/6020549/99890190-0b3f0100-2ca0-11eb-94c6-ba7e2cfe1727.jpg)


