#include <DHT.h>
#include "LowPower.h"
#include "lib.h"
#include "sensordata.h"

char sensortype[3] = {'g', 'h', '\0'};

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

  // this sensor is battery powered, sleep for a while
  for (int i=0; i<2; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
