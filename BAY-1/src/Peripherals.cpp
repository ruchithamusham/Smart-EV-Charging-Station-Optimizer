#include <Arduino.h>
#include <DHT.h>
#include "State.h"
#include "Peripherals.h"
#include "config.h"


DHT   dht(DHT_PIN , DHT_TYPE);
float mapfloat(long x , long inMin, long inMax, float outMin, float outMax)
{
      return(x-inMin)*(outMax-outMin)/(float)(inMax-inMin)+outMin;
   }


void sampleSensor(void)
{
    int raw_current = analogRead(CURRENT_PIN); // 0 to 4095     
    int raw_voltage = analogRead(VOLTAGE_PIN); // 0 to 4095       
    
    //mapping the voltage from 0 to 250V
   
    voltage= mapfloat(raw_voltage, 0, 4095, 0,250);

    if(bayStatus=="CHARGING"){
      //mapping the current from 0 to 32A

    current= mapfloat(raw_current, 0, 4095, 0, 32);

   }
    else{
      current=0;
    }
    //read current & store the 5 values in an array

    //calculate power
    power= voltage*current;
    

    //to read temperature 
    float t= dht.readTemperature(DHT_PIN );
    if(!(isnan(t)))temperature =t;

}

float recentAvgCurrent(void)
{
   //read recent 5 current values
   float sum = 0;
   for(int i=0;i<5;i++)
   {
      sum += current; 

   }
   return sum/5;

}

bool plugin_flag=1;
bool plugout_flag=1;

void plug_status(void)
{
   bool pluginReading = digitalRead(BTN_PLUGIN);
   // detect the switch is pressed
   if(pluginReading==LOW && plugin_flag)
   {
     // plug in switch is pressed
     plugin_flag=0;
   // change bay_status FREE to charging
      if(bayStatus=="FREE"){
      bayStatus="CHARGING";
      sessionStartMs=millis();            // start time of this session
      energyWh = 0;                       // new session starts at 0 Wh
      Serial.print(BAY_ID);
      Serial.println(" plugin is detected, it is charging");
      digitalWrite(RELAY_PIN, HIGH);      // turn on relay to start charging
   }
   //update leds
   }
   if(pluginReading == HIGH){
      plugin_flag=1;
   }

   bool plugoutReading = digitalRead(BTN_PLUGOUT);
   // plug out switch is pressed
   if(plugoutReading==LOW && plugout_flag)
   {
     
     plugout_flag=0;
   
   // change bay_status  charging to FREE
      if(bayStatus=="CHARGING"){
      bayStatus="FREE";
      manualOverrideActive = false;       // automatic control comes back
      throttleLevel = 100;
      loadDecision = "IDLE";
      digitalWrite(RELAY_PIN, LOW);       // stop charging
      Serial.print(BAY_ID);
      Serial.println(" plugout is detected, it is free");
   }
   
   }
   if(plugoutReading == HIGH){
      plugout_flag=1;
   }
   //update leds

}
/*void update_led_status(void){
   // if bay status is free then turn on green  led
   if (bayStatus== "FREE"){
      digitalWrite(LED_GREEN,HIGH);
      digitalWrite(LED_YELLOW,LOW);
      }
   //if bay status is charging then turn on  yellow led
    else{
      
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      
   }

}
   */