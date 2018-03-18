#include <VirtualWire.h>

// sends charBuf useing VirtualWire
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
