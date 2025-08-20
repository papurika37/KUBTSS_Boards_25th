// https://wiki.dfrobot.com/URM37_V5.0_Ultrasonic_Sensor_SKU_SEN0001_?utm_source=chatgpt.com
// BoardManager : Pololu A-Star 328pb
// https://files.pololu.com/arduino/package_pololu_index.json
// Pololu A-Star Boards by Pololu

#include <SPI.h>
#include <mcp_can.h>

#define CAN0_CS 10
#define ERR_LED  6
#define canId 0x07
#define URECHO   3
#define URTRIG   4
uint8_t sendData[8] = {0,0,0,0,0,0,0,0};
MCP_CAN CAN0(CAN0_CS);

// Measurement
uint8_t Distance_h = 0;
uint8_t Distance_l = 0;
// Temp
uint8_t temp_h = 0;
uint8_t temp_l = 0;
uint8_t EnTempCmd[4] = {0x22, 0x11, 0x00, 0x11}; // temperature measure command
uint8_t TempData[4];

void setup() {      
  Serial.begin(9600);                              
  while(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK){
    digitalWrite(ERR_LED, HIGH);
  }
  digitalWrite(ERR_LED, LOW);
  CAN0.setMode(MCP_NORMAL);   
  pinMode(URTRIG, OUTPUT);
  digitalWrite(URTRIG, HIGH);
  pinMode(URECHO, INPUT);
  delay(500);
}

void loop() {
  digitalWrite(URTRIG, LOW);
  digitalWrite(URTRIG, HIGH);
  unsigned long DistanceMeasured = pulseIn(URECHO, LOW)/50; // Measure distance by the length of pulse
  unsigned long TempValue = 0;
  getTemp();
  delay(100);

  //get data on 10hz
  sendData[0] = DistanceMeasured / 0x100;
  sendData[1] = DistanceMeasured % 0x100;
  sendData[2] = TempValue / 0x100;
  sendData[3] = TempValue % 0x100;
  uint8_t status = CAN0.sendMsgBuf(canId, 8, sendData);
  if(status != 0 && status != 6 && status != 7){
    digitalWrite(ERR_LED, HIGH);
    delay(50);
    digitalWrite(ERR_LED, LOW);
    delay(50);
  }
}

void getTemp() {
  int i;
  int TempValue;
  for (i = 0; i < 4; i++) {
    Serial.write(EnTempCmd[i]);
  }
  while (Serial.available() > 0) {
    for (i = 0; i < 4; i++) {
      TempData[i] = Serial.read();
    }
    TempValue = TempData[1] << 8;
    TempValue = TempValue + TempData[2];
  }
}