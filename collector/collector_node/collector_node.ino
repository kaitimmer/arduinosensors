#include <VirtualWire.h>
const int tx_pin = 12;

void setup() {
  Serial.begin(9600);  // Debugging only
  Serial.println("setup");
  vw_set_tx_pin(tx_pin);
  //vw_set_ptt_inverted(true);
  vw_setup(2000); 
}

byte count = 1;

char charBuf[3] = {'1','#',' '};

void loop() {
  charBuf[2] = char(count++);
  bool success = true;
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("sending:");
  Serial.print(charBuf[0]);
  Serial.print(charBuf[1]);
  Serial.println(byte(charBuf[2]));
  //vw_send((uint8_t *)charBuf, sizeof(charBuf));
  success = vw_send((uint8_t *)charBuf, 3);
  vw_wait_tx();
  if (success) {
    Serial.println("finished");
  } else {
    Serial.println("error while sending message");
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
