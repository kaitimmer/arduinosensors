#include <VirtualWire.h>
#include "LowPower.h"

// sends charBuf useing VirtualWire
void send_to_receiver(char charBuf[]) {
  // Generate a random Number between 1 and 10
  // Always read from an unconnected analog port here
  int sleep_s = (analogRead(1)%10);
  int sleep_ms = (analogRead(1)%10);

  Serial.print("wait: ");
  Serial.println((sleep_s * 1000) + (sleep_ms * 30));

  for (int i=0; i<sleep_s; i++){
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  for (int i=0; i<sleep_ms; i++){
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
  }

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
