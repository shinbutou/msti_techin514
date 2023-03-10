// Accelerometer Parameters: Start
#include "Arduino_BMI270_BMM150.h"
float accel_x, accel_y, accel_z;
// Accelerometer Parameters: End

// Pulse Sensor Parameters: Start
int pulse_pin = 3;
int onboard_red_led = 22;
int ppg_threshold = 550;
int pulse_signal; // Holding incoming raw data from the PPG sensor, ranging from 0 to 1024.
// Pulse Sensor Parameters: End

// RTC Chip Parameters: Start
#include "Arduino.h"
#include "uRTCLib.h"

uRTCLib rtc(0x68);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int hour, minute, second;
// RTC Chip Parameters: End

void setup() {
  Serial.begin(9600);

  // RTC Chip Setup: Start
  URTCLIB_WIRE.begin();
  rtc.set(0, 33, 22, 5, 9, 3, 23);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // dayOfWeek (1=Sunday, 7=Saturday)
  // RTC Chip Setup: End

  // Accelerometer Setup: Start
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  // Accelerometer Setup: Start

  Serial.println("Board Initialized!");
}

void loop() {
  // Pulse Sensor Code: Start
  pulse_signal = analogRead(pulse_pin);  // Read the PulseSensor's value.  
  // Serial.print(pulse_signal); // A good refresh rate is 100 ms.
  // Serial.print(", ");

  if(pulse_signal < ppg_threshold) { // Synchronizing the heartbeat with onboard LED.
    digitalWrite(onboard_red_led, HIGH);
  } else {
    digitalWrite(onboard_red_led, LOW);
  }
  // Pulse Sensor Code: End

  // RTC Chip Code: Start
  rtc.refresh();
  // The optimal refresh is naturally 1 second, maybe with multithreading.
  hour = rtc.hour();
  minute = rtc.minute();
  second = rtc.second();
  // Serial.print(hour);
  // Serial.print(", ");
  // Serial.print(minute);
  // Serial.print(", ");
  // Serial.print(second);
  // Serial.print(", ");
  // RTC Chip Code: Start

  // Accelerometer Code: Start
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(accel_x, accel_y, accel_z);
    // Serial.print(accel_x);
    // Serial.print(", ");
    // Serial.print(accel_y);
    // Serial.print(", ");
    // Serial.println(accel_z); // The optimal refresh rate could be 100 ms.
  }
  // Accelerometer Code: End
}
