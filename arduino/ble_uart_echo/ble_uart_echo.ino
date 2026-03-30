#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

static const char* DEVICE_NAME = "ESP32-BLE-ECHO";
static const char* SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
static const int RX_LED_PIN = LED_BUILTIN;
static const size_t MAX_RX_LINE = 1024;

BLECharacteristic* txCharacteristic = nullptr;
String rxLineBuffer;

void blinkRxLed(uint8_t times = 2, uint16_t onMs = 80, uint16_t offMs = 80) {
  for (uint8_t i = 0; i < times; ++i) {
    digitalWrite(RX_LED_PIN, HIGH);
    delay(onMs);
    digitalWrite(RX_LED_PIN, LOW);
    delay(offMs);
  }
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    Serial.println("BLE connected");
  }

  void onDisconnect(BLEServer* server) override {
    Serial.println("BLE disconnected");
    server->getAdvertising()->start();
    Serial.println("BLE advertising restarted");
  }
};

class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    String value = characteristic->getValue();
    if (value.isEmpty()) {
      return;
    }

    for (size_t i = 0; i < value.length(); ++i) {
      char c = value[i];
      if (c == '\r') {
        continue;
      }
      if (c == '\n') {
        if (!rxLineBuffer.isEmpty()) {
          Serial.print("BLE RX -> ");
          Serial.println(rxLineBuffer);
          blinkRxLed();
          rxLineBuffer = "";
        }
        continue;
      }
      rxLineBuffer += c;
      if (rxLineBuffer.length() >= MAX_RX_LINE) {
        Serial.print("BLE RX (truncated) -> ");
        Serial.println(rxLineBuffer);
        blinkRxLed();
        rxLineBuffer = "";
      }
    }

    if (txCharacteristic != nullptr) {
      txCharacteristic->setValue((uint8_t*)value.c_str(), value.length());
      txCharacteristic->notify();
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(RX_LED_PIN, OUTPUT);
  digitalWrite(RX_LED_PIN, LOW);
  Serial.println("Starting BLE UART Echo...");

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService* service = server->createService(SERVICE_UUID);

  txCharacteristic = service->createCharacteristic(
    TX_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  txCharacteristic->addDescriptor(new BLE2902());
  txCharacteristic->setValue("ready");

  BLECharacteristic* rxCharacteristic = service->createCharacteristic(
    RX_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  rxCharacteristic->setCallbacks(new RxCallbacks());

  service->start();
  BLEAdvertising* advertising = server->getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->start();

  Serial.print("BLE ready as ");
  Serial.println(DEVICE_NAME);
  Serial.println("Waiting for app writes...");
}

void loop() {
  delay(50);
}
