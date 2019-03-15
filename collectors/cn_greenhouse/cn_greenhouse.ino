#include <DHT.h>
#include "lib.h"
#include "sensordata.h"

char sensortype[3] = {'g', 'h', '\0'};

void setup() {
  //Save Power by writing all Digital IO LOW
  for (int i = 0; i < 20; i++) {
    if((i != 2) && (i != 12))//keep the pins that are in use
    pinMode(i, OUTPUT);
  }

  //SETUP WATCHDOG TIMER
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode

  //Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
  ADCSRA &= ~(1 << 7);

  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep

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
  deepSleep(40);
}

void deepSleep(int sleeptime) {
  for (int i = 0; i < sleeptime; i++) {
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");//in line assembler to go to sleep
  }
}

ISR(WDT_vect){
  //DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt
