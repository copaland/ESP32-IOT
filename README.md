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

## wokwi .com

### 1. Wokwi Simulator 설치

VScode 확장에서 Wokwi Simulator 검색 후 설치

### 2. wokwi.com

포트에 맞게 회로 그리고, 
아래 2개의 파일을 VScode 프로젝트의 루트에 붙여 넣기

### diagram.json
wokwi.com에서 회로 연결하고 생성된 diagram.json 파일 저장하여 사용  

### wokwi.toml

```
[wokwi]
version = 1
elf = ".pio/build/esp32/firmware.elf"
firmware = ".pio/build/esp32/firmware.bin"

# Forward http://localhost:8180 to port 80 on the simulated ESP32:
[[net.forward]]
from = "localhost:8180"
to = "target:80"
```
