#include <DHT.h>
#include "lib.h"
#include "sensordata.h"

char sensortype[3] = {'s', 'r', '\0'};

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
  char* result_buffer = get_result(sensortype);
  send_to_receiver(result_buffer);
}
