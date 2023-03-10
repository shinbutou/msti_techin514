int pulse_pin = 3;
int onboard_red_led = 22;
int ppg_threshold = 550;
int pulse_signal; // holding incoming raw data, ranging from 0 to 1024

void setup() {
  
}

void loop() {
  // Pulse Sensor: Start
  pulse_signal = analogRead(pulse_pin);  // Read the PulseSensor's value.
  Serial.print("Pulse: ");
  Serial.println(pulse_signal);

  if(pulse_signal < ppg_threshold) {
    digitalWrite(onboard_red_led, HIGH);
  } else {
    digitalWrite(onboard_red_led, LOW);
  }
  // Pulse Sensor: End
}
