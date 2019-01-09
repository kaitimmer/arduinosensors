#include <DHT.h>
// Temp char buffer for float values
#define TMPFLOAT 4
// Result Buffer Length
#define BUFLEN 16

// Temperature and Humidity Data Pin
#define DHTPIN 2
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22

// VirtualWire Transmit Pin
#define TXPIN 12
DHT dht(DHTPIN, DHTTYPE);

//defining the result buffer as a global variable
// +1 for the null termination
char result_buffer[BUFLEN + 1] = {0};

char* get_result(char sensortype[3]) {
  // +1 for the null termination
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
  sprintf(result_buffer, "CN;%s;%s;%s;", sensortype, temperature, humidity);

  Serial.println("result_array: ");
  Serial.println(result_buffer);

  return result_buffer;
}
