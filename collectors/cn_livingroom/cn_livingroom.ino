#include <DHT.h>
#include "lib.h"

// Temperature and Humidity Data Pin
#define DHTPIN 2
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

// VirtualWire Transmit Pin
#define TXPIN 12

// Result Buffer Length
#define BUFLEN 16

// Temp char buffer for float values
#define TMPFLOAT 4

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Debugging only
  Serial.println("setup");
  vw_set_tx_pin(TXPIN);
  Serial.println("waiting for 5 sec to let sensors initialize");
  delay(5000);
  vw_setup(2000);
}

void loop() {
  Serial.println("#### New Loop Run ####");
  // +1 for the null termination
  char result_buffer[BUFLEN + 1] = {0};
  char temperature[TMPFLOAT + 1] = {0};
  char humidity[TMPFLOAT + 1] = {0};

  float f_humidity = 0.0;
  float f_temperature = 0.0;

  f_humidity = dht.readHumidity() / 100;
  f_temperature = dht.readTemperature();

  if (!isnan(f_humidity)) {
    dtostrf(f_humidity, TMPFLOAT, 2, humidity);
  } else {
    fill_array(humidity, 'E', TMPFLOAT);
  }

  if (!isnan(f_temperature)) {
    dtostrf(f_temperature, TMPFLOAT, 1, temperature);
  } else {
    fill_array(temperature, 'E', TMPFLOAT);
  }

 Serial.print("temp: ");
 Serial.println(temperature);
 Serial.print("humidity: ");
 Serial.println(humidity);

  //collector_id;temperature;humidity;
  sprintf(result_buffer, "CN;lr;%s;%s;", temperature, humidity);

  Serial.println("result_array: ");
  Serial.println(result_buffer);

  send_to_receiver(result_buffer);

  //delay(10000);
  //delay(1000);
}
