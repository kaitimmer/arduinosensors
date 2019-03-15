#include <VirtualWire.h>

// sends charBuf useing VirtualWire
void send_to_receiver(char charBuf[]) {
  digitalWrite(LED_BUILTIN, HIGH);
  vw_send((uint8_t *)charBuf, strlen(charBuf));
  vw_wait_tx();
  digitalWrite(LED_BUILTIN, LOW);
}

void fill_array(char a[], char fill, int len) {
  for (int i = 0; i < len; i++ ) {
    a[i] = fill;
  }
}
