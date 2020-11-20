# esp-idf-http-camera
Take a picture and Publish it via HTTP.   

![スライド1](https://user-images.githubusercontent.com/6020549/99767668-c5225a00-2b46-11eb-826e-a5dd106544fe.JPG)

![スライド2](https://user-images.githubusercontent.com/6020549/99767675-c81d4a80-2b46-11eb-9712-ec0be23d1685.JPG)

# Server Side
I tested with Tornado & Flask.   
You can choose the one you like.   

## Using Tornado

### Install Tornado
```
sudo apt install python-pip python-setuptools
python -m pip install -U pip
python -m pip install -U wheel
python -m pip install tornado
```

### Start WEB Server using Tornado
```
git clonse https://github.com/nopnop2002/esp8266-multipart
cd esp8266-multipart/tornado
python upload.py
```

---

## Using Flask

### Install Flask
```
sudo apt install python-pip python-setuptools
python -m pip install -U pip
python -m pip install -U wheel
python -m pip install flask
```

### Start WEB Server using Flask
```
git clonse https://github.com/nopnop2002/esp8266-multipart
cd esp8266-multipart/flask
python upload.py
```

---

# ESP32 Side

## Software requirements
esp-idf ver4.1 or later.   

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

![config-app](https://user-images.githubusercontent.com/6020549/99767823-074b9b80-2b47-11eb-9e42-284b108bde41.jpg)

### Wifi Setting

![config-wifi](https://user-images.githubusercontent.com/6020549/66692062-e4a94b00-ecd5-11e9-9ea7-afb74cc347af.jpg)

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

![config-shutter-1](https://user-images.githubusercontent.com/6020549/98636952-1e320700-236b-11eb-8c5b-0a2b56267310.jpg)

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

![config-shutter-2](https://user-images.githubusercontent.com/6020549/98636945-1c684380-236b-11eb-8682-32e8430d7216.jpg)

- Shutter is TCP Socket   
You can connect with mDNS hostname.   
You can use tcp_send.py.   
```
python ./tcp_send.py
```

![config-shutter-3](https://user-images.githubusercontent.com/6020549/98636948-1d997080-236b-11eb-8579-b312da1ac915.jpg)

- Shutter is UDP Socket   
You can use udp_send.py.   
```
python ./udp_send.py
```

![config-shutter-4](https://user-images.githubusercontent.com/6020549/98636950-1d997080-236b-11eb-98ca-c14d16954f42.jpg)

### Flash Light

ESP32-CAM by AI-Thinker have flash light on GPIO4.

![config-flash](https://user-images.githubusercontent.com/6020549/98637034-46216a80-236b-11eb-8504-e83f718f5e85.jpg)


