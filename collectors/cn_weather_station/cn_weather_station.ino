#include <VirtualWire.h>
#include <DHT.h>

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

  f_humidity = dht.readHumidity();
  f_temperature = dht.readTemperature();

  if (!isnan(f_humidity)) {
    dtostrf(f_humidity, TMPFLOAT, 1, humidity);
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
  sprintf(result_buffer, "CN;ws;%s;%s;", temperature, humidity);

  Serial.println("result_array: ");
  Serial.println(result_buffer);

  send_to_receiver(result_buffer);

  //wait a bit before next loop
  delay(2000);
}

/*void print_array(char a[], int len){
  for(int i=0; i<len; i++){
    Serial.print(a[i]);
  }
  Serial.println(' ');
}*/

void send_to_receiver(char charBuf[]) {
  // Generate a random Number between 1000 and 10000
  // Always read from an unconnected analog port here
  int r = (analogRead(1)%100)*100;

  Serial.print("wait: ");
  Serial.println(r);
  delay(r);

  Serial.println("sending...");
  digitalWrite(LED_BUILTIN, HIGH);
  vw_send((uint8_t *)charBuf, strlen(charBuf));
  vw_wait_tx();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("finished sending...");
}

void fill_array(char a[], char fill, int len) {
  for (int i = 0; i < len; i++ ) {
    a[i] = fill;
  }
}
