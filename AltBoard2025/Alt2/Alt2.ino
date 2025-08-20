#include <SPI.h>
#include <mcp_can.h>

#define CAN0_CS 10 // MCP25625
#define ERR_LED 6  // indicator

#define canId 0x07 //CAN ID

const long SENSOR_INTERVAL = 50; // interval

MCP_CAN CAN0(CAN0_CS);

// flag for data (temp/alt)
bool measureData = false; 

unsigned int g_distance = 0;
unsigned int g_temperature = 0.0;

unsigned long previousMillis = 0;

void setup() {
  pinMode(ERR_LED, OUTPUT);
  digitalWrite(ERR_LED, LOW);
  Serial.begin(9600);
  delay(500);

  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
    digitalWrite(ERR_LED, HIGH);
    while (1);
  }
  CAN0.setMode(MCP_NORMAL);
}


void loop() {
  requestSensorReading();   // get data
  readSensorAndSendData();  // sending
}

void requestSensorReading() {
  if (millis() - previousMillis >= SENSOR_INTERVAL) {
    previousMillis = millis();

    if (measureData) {
      // command for temp
      uint8_t tempCommand[4] = {0x11, 0x00, 0x00, 0x11};
      Serial.write(tempCommand, 4);
    } else {
      // command for dist
      uint8_t distCommand[4] = {0x22, 0x00, 0x00, 0x22};
      Serial.write(distCommand, 4);
    }
    // change flag
    measureData = !measureData;
  }
}

void readSensorAndSendData() {
  if (Serial.available() >= 4) {
    uint8_t responseData[4];
    Serial.readBytes(responseData, 4);

    bool newDataReceived = false;

    // judge data by header command
    if (responseData[0] == 0x22) { // dist
      g_distance = (responseData[1] << 8) + responseData[2];
      newDataReceived = true;

    } else if (responseData[0] == 0x11) { // temp
      g_temperature = (responseData[1] << 8) + responseData[2];
      newDataReceived = true;
    }

    if (newDataReceived) {
      uint8_t sendData[8] = {0};
      
      // dist(2 byte)
      sendData[0] = highByte(g_distance);
      sendData[1] = lowByte(g_distance);
      
      // temp(2 byte)
      sendData[2] = highByte(g_temperature);
      sendData[3] = lowByte(g_temperature);

      // sending
      byte sndStats = CAN0.sendMsgBuf(canId, 0, 8, sendData);
      if (sndStats != 0 && sndStats != 6 && sndStats != 7) {
        digitalWrite(ERR_LED, HIGH);
        delay(50);
        digitalWrite(ERR_LED, LOW);
      }
    }
  }
}