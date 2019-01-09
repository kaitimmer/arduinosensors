#include <VirtualWire.h>
#include <Ethernet.h>
#include "secret.h"

#define RXPIN 2

// ethernet shield mac address
byte mac[] = {  0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0x01 };

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer SERVER(80);

char DELIMITER[] = ";";

// global measurement variables

// ws (weather station)
char ws_temperature_celsius[5] = {0};
char ws_humidity_percent[5] = {0};
char ws_last_received_millis[12] = {0};

// lr (living room)
char lr_temperature_celsius[5] = {0};
char lr_humidity_percent[5] = {0};
char lr_last_received_millis[12] = {0};

// sr (storage room)
char sr_temperature_celsius[5] = {0};
char sr_humidity_percent[5] = {0};
char sr_last_received_millis[12] = {0};

void setup()
{
  Serial.begin(9600);  // Debugging only
  Serial.println("initializing....");

  // start the Ethernet connection:
  Serial.println("Starting Ethernet Connection");
  Ethernet.begin(mac, ip);

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

  // wait for 100ms
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
  listenForEthernetClients();
}

void listenForEthernetClients(){
  char linebuf[80];
  int charcount=0;
  boolean authenticated=false;

  // listen for incoming clients
  EthernetClient client = SERVER.available();
  if (client) {
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    authenticated=false;

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        linebuf[charcount]=c;
        if (charcount < sizeof(linebuf) -1){
          charcount++;
        }

        Serial.write(c);

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          if (authenticated) {
            sendMetrics(client);
          } else {
            sendAuthpage(client);
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          if (strcasestr(linebuf,"Authorization: Basic") > 0 && strstr(linebuf,AUTHSECRET) > 0) {
            authenticated=true;
          }
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;
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

void sendMetrics(EthernetClient &client){
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connnection: close");
  client.println();

  // print the current living room readings, in prometheus format:
  client.print("temperature_celsius{collector=\"livingroom\", sensor=\"dht22\"} ");
  client.println(lr_temperature_celsius);
  client.print("humidity_percent{collector=\"livingroom\", sensor=\"dht22\"} ");
  client.println(lr_humidity_percent);
  client.print("last_received_millis{collector=\"livingroom\"} ");
  client.println(lr_last_received_millis);
  // print the current storage room readings, in prometheus format:
  client.print("temperature_celsius{collector=\"storageroom\", sensor=\"dht22\"} ");
  client.println(sr_temperature_celsius);
  client.print("humidity_percent{collector=\"storageroom\", sensor=\"dht22\"} ");
  client.println(sr_humidity_percent);
  client.print("last_received_millis{collector=\"storageroom\"} ");
  client.println(sr_last_received_millis);
}

void sendAuthpage(EthernetClient &client)
{
  client.println("HTTP/1.1 401 Authorization Required");
  client.println("WWW-Authenticate: Basic realm=\"Secure Area\"");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<HTML><HEAD><TITLE>Error</TITLE>");
  client.println(" </HEAD><BODY><H1>401 Unauthorized.</H1></BODY></HTML>");
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
      if (strcmp(type, "ws") == 0) {
        // weather station
        switch (count){
          default:
            continue;
        }
        sprintf(ws_last_received_millis, "%lu", millis());

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
        sprintf(lr_last_received_millis, "%lu", millis());

      } else if (strcmp(type, "sr") == 0) {
        // storage room
        switch (count) {
          case 2:
            //temperature
            strncpy(sr_temperature_celsius, ptr, 4);
            ;;
          case 3:
            //humidity
            strncpy(sr_humidity_percent, ptr, 4);
            ;;
          default:
            ;;
        }
        sprintf(sr_last_received_millis, "%lu", millis());

      } else {
        Serial.println("Wooot just happened (type unkown)");
      }
    }
    // next part
    ptr = strtok(NULL, DELIMITER);
    count++;
  }
}
