#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

int PulseSensorSignalPin = 26;
int Signal;
 
void setup() {
  SerialBT.begin("ESP32_shinbutou");
}
 
void loop() {
  Signal = analogRead(PulseSensorSignalPin);
  SerialBT.println(Signal);
  delay(200);
}