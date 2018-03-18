#include <VirtualWire.h>
#include <Ethernet.h>

#define RXPIN 2

// ethernet shield mac address
byte mac[] = {  0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0x01 };

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer SERVER(80);

unsigned long LAST_DHCP_CHECK;
char DELIMITER[] = ";";

// global measurement variables

// ws (weather station)
char ws_temperature_celsius[5] = {0};
char ws_humidity_percent[5] = {0};

// lr (living room)
char lr_temperature_celsius[5] = {0};
char lr_humidity_percent[5] = {0};

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
  vw_wait_rx_max(100);

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(LED_BUILTIN, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("received: ");

    for (int i = 0; i < buflen; i++)
    {
      // create char array from received data
      charBuf[i] = (char)buf[i];
    }
    Serial.println(charBuf);

    parseLine(charBuf);

    digitalWrite(LED_BUILTIN, LOW);
  }
  checkDHCP();
  listenForEthernetClients();
}

void listenForEthernetClients(){
  // listen for incoming clients
  EthernetClient client = SERVER.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println();

          // print the current readings, in HTML format:
          client.print("temperature_celsius{collector=livingroom, sensor=dht22} ");
          client.println(lr_temperature_celsius);
          client.print("humidity_percent{collector=livingroom, sensor=dht22} ");
          client.println(lr_humidity_percent);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

void parseLine(char charBuf[]){
  int count = 0;
  char type[3] = {0};
  char *ptr = strtok(charBuf, DELIMITER);

  while(ptr != NULL) {
    if (count == 0){ // global type of collector
      if (strcmp(ptr, "CN") != 0) {
        Serial.println("not for us, stop parsing!");
        // stop this is not for us
        break;
      }
    } // type
    else if (count == 1) {
      strncpy(type, ptr, 2);
    } else {
      Serial.print("count: ");
      Serial.println(count);

      if (strcmp(type, "ws") == 0) {
        // weather station
        switch (count){
          default:
            continue;
        }
      } else if (strcmp(type, "lr") == 0) {
        // living room
        switch (count) {
          case 2:
            //temperature
            strncpy(lr_temperature_celsius, ptr, 4);
            ;;
          case 3:
            //humidity
            strncpy(lr_humidity_percent, ptr, 4);
            ;;
          default:
            ;;
        }
      } else {
        Serial.println("Wooot just happened (type unkown)");
      }
    }
    // next part
    ptr = strtok(NULL, DELIMITER);
    count++;
  }
}

void checkDHCP() {
  if ((millis() - LAST_DHCP_CHECK) > 3600000){
    Serial.println("maintain dhcp address");
    Ethernet.maintain();
  }

  LAST_DHCP_CHECK = millis();
}
