#include <VirtualWire.h>
#include <Ethernet.h>

#define RXPIN 12

// ethernet shield mac address
byte mac[] = {  0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0x01 };

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer SERVER(80);

unsigned long LAST_DHCP_CHECK;

void setup()
{
  Serial.begin(9600);  // Debugging only
  Serial.println("initializing....");

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // initialize the Ethernet device not using DHCP:
    Ethernet.begin(mac);
  }

  LAST_DHCP_CHECK = millis();

  // print your local IP address:
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  // start listening for clients
  SERVER.begin();

  // 44mhz receiver initialize
  vw_set_rx_pin(RXPIN);
  vw_setup(2000);      // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
  Serial.println("done");
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  char charBuf[buflen] = {0};

  // wait for a max of 30 mins, then go on and at least check the DHCP lease
  // even when nothing arrived on the RF Port
  vw_wait_rx_max(300000);

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(LED_BUILTIN, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");

    for (int i = 0; i < buflen; i++)
    {
      // create char array from received data
      charBuf[i] = (char)buf[i];
    }
    Serial.println(charBuf);

    // check if this is a message from a collector node
    if (charBuf[0] == 'C' && charBuf[1] == 'N'){

    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  checkDHCP();
}

void checkDHCP() {
  if ((millis() - LAST_DHCP_CHECK) > 3600000){
    Serial.println("maintain dhcp address");
    Ethernet.maintain();
  }

  LAST_DHCP_CHECK = millis();
}
