## Easy Module Shield V1 

| S3 | UNO | 설명 |
| -------- | -------- | -------- |
|44|	D0 |rx |
|43|	D1 | tx |
|8|	D2 |SW1  버튼 |	
|9|	D3 |SW2  버튼 |	
|10|	D4 |DHT11 온도 측정|	
|11|	D5 |Buzzer 알람|	
|12|	D6 | IR Receiver 적외선 리모컨| 
|13 |	D7 |J1|
|14|	D8 |J2|
|15|	D9 |RGB LED red	|
|16|	D10 |RGB LED blue	|
|21|	D11 |RGB LED green|
|38|	D12 |LED2  빨강	LED|
|47|	D13 |LED1 파랑	LED|
|1|	A0 |POT 	가변 저항|
|2|	A1 | Light 	조도 센서|
|4|	A2 |LM35d 	온도 측정|
|5|	A3 |J3|

## VScode 사용

### 1. VScode 설치
[https://code.visualstudio.com/](https://code.visualstudio.com/)

### 2. VScode에 확장 설치
```
Korean Language Pack for Visual Studio Code
Live Server
npm Intellisense
PlatformIO IDE
Wokwi Simulator
GitHub Pull Requests & GitLens — Git supercharged
GitHub Copilot
Gemini Code Assist
```

### 3. 프로젝트 생성

탐색기 선택(왼쪽 위)  
  a. 폴더 생성 : edu  
  b. 파일 - 작업영역에 폴더추가  
  
Platformio 선택  
  a. QUICK ACCESS - PIO Home - Open - Home  
  b. +New Project  
  c. Project Wizard  
    - Name: e01-name, Board: espressif esp32-s3-devkitc-1-N8, Framework: Arduino  
  d. Finish  
  
Platformio.ini  
```
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200
lib_deps = 
	adafruit/DHT sensor library@^1.4.6
	lib/xht11
	marcoschwartz/LiquidCrystal_I2C@^1.1.4
```
## VScode에 코드 작성

프로젝트 폴더 - src - main.cpp 에서 코드 작성  
`#include <Arduino.h>` 가 포함되도록 한다.  

## VScode wokwi 시뮬레이션

### 1. Wokwi Simulator 설치  
VScode 확장에서 Wokwi Simulator 검색 후 설치  

### 2. wokwi.com
포트에 맞게 회로를 작성하고, diagram.json 탭을 선택하고 복사한다.  
VScode에서는 아래 2개의 파일이 필요하므로 프로젝트의 루트에 붙여넣기한다.  

### 3. diagram.json
wokwi.com에서 회로 연결하고 생성된 diagram.json 파일 저장하여 사용  

### 4. wokwi.toml

ESP32  
```
[wokwi]
version = 1
elf = ".pio/build/esp32/firmware.elf"
firmware = ".pio/build/esp32/firmware.bin"

[[net.forward]]
from = "localhost:8180"
to = "target:80"
```

ESP32-S3  
```
[wokwi]
version = 1
elf = ".pio/build/esp32-s3-devkitc-1/firmware.elf"
firmware = ".pio/build/esp32-s3-devkitc-1/firmware.bin"

# Forward http://localhost:8180 to port 80 on the simulated ESP32:
[[net.forward]]
from = "localhost:8180"
to = "target:80"
protocol = "http"
```

## 아두이노 라이브러리 추가

### 1. Platformio 선택
a. QUICK ACCESS - PIO Home - Open - Libraries  
b. 적합한 라이브러리 선택  
c. 설치할 프로젝트 선택  

### 2. 다운받은 라이브러리 설치
a. 프로젝트 위에서 RMB - 파일 탐색기에 표시  
b. lib 폴더 안에 압축해제한 라이브러리를 폴더째 북사하여 붙여넣기한다.  
