const int x_pin = 0;
const int y_pin = 1;
const int z_pin = 2;
int raw_min = 0;
int raw_max = 1023;
const int sample_size = 10;

int pulse_pin = 3;
int LED = 13; // the on-board LED
int pulse_signal; // holding incoming raw data, ranging from 0 to 1024
int threshold = 550;

// AM2302 | Source: https://www.makerguides.com/dht11-dht22-arduino-tutorial/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22 // AM2302

DHT dht = DHT(DHTPIN, DHTTYPE);

void setup()
{
  pinMode(A3, INPUT);
  pinMode(LED, OUTPUT); // blinking LED according to the heartbeat
  dht.begin();

  Serial.begin(9600); // setting up serial communication at certain speed
}

void loop() {
  int x_raw = get_axis(x_pin);
  int y_raw = get_axis(y_pin);
  int z_raw = get_axis(z_pin);

  long x_scaled = map(x_raw, raw_min, raw_max, -3000, 3000);
  long y_scaled = map(y_raw, raw_min, raw_max, -3000, 3000);
  long z_scaled = map(z_raw, raw_min, raw_max, -3000, 3000);

  float x_avalue = x_scaled / 1000.0;
  float y_avalue = y_scaled / 1000.0;
  float z_avalue = z_scaled / 1000.0;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  Serial.print("X, Y, Z :: ");
  Serial.print(x_raw);
  Serial.print(", ");
  Serial.print(y_raw);
  Serial.print(", ");
  Serial.print(z_raw);
  Serial.print(" :: ");
  Serial.print(x_avalue, 0);
  Serial.print("G, ");
  Serial.print(y_avalue, 0);
  Serial.print("G, ");
  Serial.print(z_avalue, 0);
  Serial.print("G, ");
  Serial.println("");

  // Pulse Sensor
  pulse_signal = analogRead(pulse_pin);  // Read the PulseSensor's value.
  Serial.print("Pulse: ");
  Serial.println(pulse_signal);

  if(pulse_signal > threshold) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
  
  // Humidity & Temperature
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(f);
  Serial.print(" \xC2\xB0");
  Serial.print("F ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" \xC2\xB0");
  Serial.print("C | ");
  Serial.print(hif);
  Serial.print(" \xC2\xB0");
  Serial.println("F"); 

  delay(1000);
}

// Gyroscope
int get_axis(int pin)
{
  long value = 0;
  analogRead(pin);
  delay(1);

  for(int i = 0; i < sample_size; i++)
    {
      value += analogRead(pin);
    }
  return value / sample_size;
}