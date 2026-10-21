#ifndef PERIPHERALS_H
#define PERIPHERALS_H
#include <DHT.h>

extern DHT dht;


void printval(void);
void plug_status(void);
void update_led_status(void);
void sampleSensor(void);
float recentAvgCurrent(void);

#endif