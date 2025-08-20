#include <BluetoothSerial.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <WiFi.h>

//BLE Characteristics.
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "abcd1234-e1ba-e1ba-e1ba-123456789abc"
#define BLE_DEVICE_NAME     "KUBTSS-ESP32"
// BLE status.
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long oldNotifyTime = 0; //count time from last notification.

BLEServer *pServer = NULL; //initiate as NULL
BLECharacteristic *pCharacteristic = NULL;

uint8_t logNum = 0;
bool playerIsUsed;
unsigned long oldSoundPlayTime = 0;
unsigned long nowTime = 0;


class KUBTServerCallbacks : public BLEServerCallbacks{
  void onConnect(BLEServer *pServer){
    deviceConnected = true;
    Serial.println("Connect");
  };
  void onDisconnect(BLEServer *pServer){
    deviceConnected = false;
    Serial.println("Disconnect");
  }
};


//SETUP and LOOP below------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  //while(!Serial);
  while(nowTime<= 1000){
    nowTime = millis();
  }
  //wifi powered off
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Turn off Wi-Fi.");
  
  //initiate BLE device
  BLEDevice::init(BLE_DEVICE_NAME);
  //create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new KUBTServerCallbacks()); //pServerにCallback関数を設定
  BLEService *pService = pServer->createService(SERVICE_UUID); //pServerにserviceuuidを設定
  /*
  create BLE characteristics
  write below what service can do to characteristic
  https://yegang.hatenablog.com/entry/2014/08/09/195246
  get notify when data changed
  */

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic -> addDescriptor(new BLE2902());
  //start BLE service
  pService -> start();
  // start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  // add properties
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Begin serial communication with 115200 board rates.");
  Serial.println("Create server.");
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // put your main code here, to run repeatedly:
  sendData();
  delay(1000);
}

void sendData(){
  //check BLE-connection state.
  //reConnectBle();

  if(deviceConnected){
    Serial.println('Send');
    uint8_t sensorDataArray[10]={
      0,1,2,3,4,5,6,7,8,9
    };
    //約500msおきにデータを送信する処理
    while(oldNotifyTime+500 > millis()){}//待機処理
    pCharacteristic->setValue((uint8_t*)&sensorDataArray, 10);
    pCharacteristic->notify(); //send data to phone
    oldNotifyTime = millis();
  }
}
