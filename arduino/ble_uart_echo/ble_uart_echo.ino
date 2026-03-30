#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLESecurity.h>
#include <esp_gap_ble_api.h>
#include <esp_system.h>
#include <stdlib.h>

static const char* DEVICE_NAME = "ESP32-BLE-ECHO";
static const char* SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
static const int RX_LED_PIN = LED_BUILTIN;
static const size_t MAX_RX_LINE = 1024;
static bool g_deviceConnected = false;
static uint32_t g_lastAdvKickMs = 0;
static bool g_restartAdvertisingRequested = false;
static uint32_t g_restartAdvertisingAtMs = 0;
static bool g_clearBondsRequested = false;

BLECharacteristic* txCharacteristic = nullptr;
String rxLineBuffer;

void sendTxMessage(const String& message) {
  if (txCharacteristic == nullptr || !g_deviceConnected || message.isEmpty()) {
    return;
  }
  txCharacteristic->setValue((uint8_t*)message.c_str(), message.length());
  txCharacteristic->notify();
  Serial.print("BLE TX -> ");
  Serial.println(message);
}

void blinkRxLed(uint8_t times = 2, uint16_t onMs = 80, uint16_t offMs = 80) {
  for (uint8_t i = 0; i < times; ++i) {
    digitalWrite(RX_LED_PIN, HIGH);
    delay(onMs);
    digitalWrite(RX_LED_PIN, LOW);
    delay(offMs);
  }
}

const char* resetReasonLabel(esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_POWERON: return "POWERON";
    case ESP_RST_SW: return "SW";
    case ESP_RST_PANIC: return "PANIC";
    case ESP_RST_INT_WDT: return "INT_WDT";
    case ESP_RST_TASK_WDT: return "TASK_WDT";
    case ESP_RST_WDT: return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    case ESP_RST_SDIO: return "SDIO";
    default: return "UNKNOWN";
  }
}

void requestAdvertisingRestart(uint32_t delayMs = 200) {
  g_restartAdvertisingRequested = true;
  g_restartAdvertisingAtMs = millis() + delayMs;
}

void clearAllBonds() {
  int devNum = esp_ble_get_bond_device_num();
  if (devNum <= 0) {
    return;
  }
  esp_ble_bond_dev_t* devList = (esp_ble_bond_dev_t*)malloc(sizeof(esp_ble_bond_dev_t) * devNum);
  if (devList == nullptr) {
    return;
  }
  if (esp_ble_get_bond_device_list(&devNum, devList) == ESP_OK) {
    for (int i = 0; i < devNum; ++i) {
      esp_ble_remove_bond_device(devList[i].bd_addr);
    }
  }
  free(devList);
}

class SecurityCallbacks : public BLESecurityCallbacks {
  void onAuthenticationComplete(esp_ble_auth_cmpl_t desc) override {
    if (!desc.success) {
      Serial.printf("BLE auth failed (0x%02x); scheduling bond clear and advertising restart\n", desc.fail_reason);
      g_clearBondsRequested = true;
      requestAdvertisingRestart(300);
    } else {
      Serial.println("BLE auth success");
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    g_deviceConnected = true;
    Serial.println("BLE connected");
  }

  void onDisconnect(BLEServer* server) override {
    g_deviceConnected = false;
    Serial.println("BLE disconnected; scheduling advertising restart");
    requestAdvertisingRestart(250);
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
          sendTxMessage("ACK:" + rxLineBuffer);
          rxLineBuffer = "";
        }
        continue;
      }
      rxLineBuffer += c;
      if (rxLineBuffer.length() >= MAX_RX_LINE) {
        Serial.print("BLE RX (truncated) -> ");
        Serial.println(rxLineBuffer);
        blinkRxLed();
        sendTxMessage("ACK:TRUNCATED");
        rxLineBuffer = "";
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(RX_LED_PIN, OUTPUT);
  digitalWrite(RX_LED_PIN, LOW);
  Serial.println("Starting BLE UART Echo...");
  Serial.printf("Reset reason: %s\n", resetReasonLabel(esp_reset_reason()));

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService* service = server->createService(SERVICE_UUID);

  txCharacteristic = service->createCharacteristic(
    TX_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  txCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
  txCharacteristic->addDescriptor(new BLE2902());
  txCharacteristic->setValue("ready");

  BLECharacteristic* rxCharacteristic = service->createCharacteristic(
    RX_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  rxCharacteristic->setAccessPermissions(ESP_GATT_PERM_WRITE_ENCRYPTED);
  rxCharacteristic->setCallbacks(new RxCallbacks());

  BLESecurity::setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  BLESecurity::setCapability(ESP_IO_CAP_NONE);
  BLESecurity::setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  BLEDevice::setSecurityCallbacks(new SecurityCallbacks());

  service->start();
  BLEAdvertising* advertising = server->getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMinPreferred(0x12);
  advertising->start();
  g_lastAdvKickMs = millis();

  Serial.print("BLE ready as ");
  Serial.println(DEVICE_NAME);
  Serial.println("Bonding required for secure writes.");
  Serial.println("Waiting for app writes...");
}

void loop() {
  if (g_clearBondsRequested) {
    g_clearBondsRequested = false;
    Serial.println("Clearing stored BLE bonds");
    clearAllBonds();
  }

  if (!g_deviceConnected && g_restartAdvertisingRequested && millis() >= g_restartAdvertisingAtMs) {
    g_restartAdvertisingRequested = false;
    BLEDevice::startAdvertising();
    g_lastAdvKickMs = millis();
    Serial.println("BLE advertising restarted");
  }

  if (!g_deviceConnected && (millis() - g_lastAdvKickMs) > 15000) {
    BLEDevice::startAdvertising();
    g_lastAdvKickMs = millis();
  }
  delay(50);
}
