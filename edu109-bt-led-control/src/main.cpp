//h2zero/NimBLE-Arduino@^2.3.0
#include <Arduino.h>
#include <NimBLEDevice.h>

// LED가 연결된 핀 번호 정의
#define LED_PIN 38

// BLE 서비스 및 특성(Characteristic) UUID
// 고유한 UUID를 생성하여 사용하세요. (예: https://www.uuidgenerator.net/)
#define SERVICE_UUID        "2439b2cd-11b3-4291-a77a-494a05ac9f26" // 서비스 UUID
#define CHARACTERISTIC_UUID "3d715a6a-9204-4592-8129-7464afd11260" // 특성 UUID

NimBLECharacteristic* pCharacteristic;

// 전역 변수 추가
static bool deviceConnected = false;
static bool oldDeviceConnected = false;
static NimBLEServer* pServer = nullptr;

// BLE 서버 콜백 클래스 추가
class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        deviceConnected = true;
        Serial.println("클라이언트가 연결되었습니다!");
    }

    void onDisconnect(NimBLEServer* pServer) {
        deviceConnected = false;
        Serial.println("클라이언트가 연결해제되었습니다!");
        // 재연결을 위한 광고 시작
        NimBLEDevice::startAdvertising();
    }
};

// LED 콜백 클래스 수정
class LEDCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if(value.length() > 0) {
            Serial.print("수신된 값: ");
            Serial.println(value.c_str());
            
            if(value == "on") {
                digitalWrite(LED_PIN, HIGH);
                Serial.println("LED ON");
            } else if(value == "off") {
                digitalWrite(LED_PIN, LOW);
                Serial.println("LED OFF");
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("BLE LED 제어 시작");

    NimBLEDevice::init("ESP32-S3_LED_Control");
    NimBLEDevice::setDeviceName("ESP32-S3_LED_Control");

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    pCharacteristic->setCallbacks(new LEDCharacteristicCallbacks());
    pCharacteristic->setValue("off");

    pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setName("ESP32-S3_LED_Control"); // ★ 광고 데이터에 이름 명시적으로 추가

    NimBLEDevice::startAdvertising();
    Serial.println("광고 시작... ESP32-S3_LED_Control를 검색하세요.");
}

void loop() {
    // 연결 상태 변경 감지 및 처리
    if (deviceConnected != oldDeviceConnected) {
        if (!deviceConnected) {
            delay(500); // 연결 해제 후 잠시 대기
            pServer->startAdvertising(); // 광고 재시작
            Serial.println("광고 재시작...");
        }
        oldDeviceConnected = deviceConnected;
    }
    delay(20);
}