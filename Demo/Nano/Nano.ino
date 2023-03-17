// Accelerometer Parameters: Start
#include "Arduino_BMI270_BMM150.h"
float accel_x_a, accel_y_a, accel_z_a;
float accel_x_b, accel_y_b, accel_z_b;
// Accelerometer Parameters: End

// Temperature/Humidity Sensor Parameters: Start
#include <Arduino_HS300x.h>
float temperature;
float temperature_array[10] = {0};
// Temperature/Humidity Sensor Parameters: End

// Pulse Sensor Parameters: Start
int pulse_pin = 3;
int onboard_red_led = 22;
int ppg_threshold = 550;
int pulse_signal;  // Holding incoming raw data from the PPG sensor, ranging from 0 to 1024.
int ppg_data[600] = {0};
// Pulse Sensor Parameters: End

// RTC Chip Parameters: Start
#include "Arduino.h"
#include "uRTCLib.h"

uRTCLib rtc(0x68);

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int hour, minute, second;
// RTC Chip Parameters: End

// Vibrator Parameters: Start
#define vibrator_pin 2
// Vibrator Parameters: End

void setup() {
  Serial.begin(9600);

  // RTC Chip Setup: Start
  URTCLIB_WIRE.begin();
  rtc.set(0, 11, 7, 6, 10, 3, 23);
  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
  // dayOfWeek (1=Sunday, 7=Saturday)
  // RTC Chip Setup: End

  // Accelerometer Setup: Start
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  // Serial.print("Accelerometer sample rate = ");
  // Serial.print(IMU.accelerationSampleRate());
  // Serial.println(" Hz");
  // Accelerometer Setup: Start

  // Temperature/Humidity Sensor Setup: Start
  if (!HS300x.begin()) {
    Serial.println("Failed to initialize temperature/humidity sensor!");
  }
  // Temperature/Humidity Sensor Setup: End

  // Vibrator Setup: Start
  pinMode(vibrator_pin, OUTPUT);
  digitalWrite(vibrator_pin, LOW);
  // Vibrator Setup: End

  Serial.println("Board Initialized!");
}

void loop() {
  int t = millis();

  // Pulse Sensor Code: Start
  pulse_signal = analogRead(pulse_pin);  // Read the PulseSensor's value.

  if (pulse_signal < ppg_threshold) {  // Synchronizing the heartbeat with onboard LED.
    digitalWrite(onboard_red_led, HIGH);
  } else {
    digitalWrite(onboard_red_led, LOW);
  }

  if (t % 100 == 0) {

  }
  // Pulse Sensor Code: End
  
  if (t % 1000 == 0) {
    // Pulse Sensor Code: Start
    Serial.print("PPG: ");
    Serial.print(pulse_signal); // A good refresh rate is 100 ms.
    Serial.print("; ");
    // Pulse Sensor Code: End

    // RTC Chip Code: Start
    rtc.refresh();
    // The optimal refresh is naturally 1 second, maybe with multithreading.
    hour = rtc.hour();
    minute = rtc.minute();
    second = rtc.second();
    Serial.print("Time: ");
    Serial.print(hour);
    Serial.print(": ");
    Serial.print(minute);
    Serial.print(": ");
    Serial.print(second);
    Serial.print("; ");
    // RTC Chip Code: End

    // Accelerometer Code: Start
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(accel_x_a, accel_y_a, accel_z_a);
      Serial.print("Accelerometer: ");
      Serial.print(accel_x_a);
      Serial.print(", ");
      Serial.print(accel_y_a);
      Serial.print(", ");
      Serial.print(accel_z_a);
      Serial.print(", "); // The optimal refresh rate could be 100 ms.

      int delta_x = abs(accel_x_b - accel_x_a);
      int delta_y = abs(accel_y_b - accel_y_a);
      int delta_z = abs(accel_z_b - accel_z_a);
      
      int delta_m = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;

      Serial.print(delta_m);
      Serial.print("; ");

      if (delta_m > 33333333 && t >= 3333) {
        // Vibrator Code: Start
        digitalWrite(vibrator_pin, HIGH);
        // Vibrator Code: End
      }

      accel_x_b = accel_x_a;
      accel_y_b = accel_y_a,
      accel_z_b = accel_z_a;
    }
    // Accelerometer Code: End

    // Temperature/Humidity Sensor Code: Start
    temperature = HS300x.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("; "); // Humidity in percentage.
    // Temperature/Humidity Sensor Code: End
  }
}
