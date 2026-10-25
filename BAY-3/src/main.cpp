#include <Arduino.h>
#include "State.h"
#include "config.h"
#include "Peripherals.h"
#include <Wifi.h>
#include "network.h"
#include "telemetery.h"
#include "model.h"
#include "edge_ai.h"
#include "optimization.h"
#include "attributes.h"
#include "rpc.h"




void setup()
{

    Serial.begin(115200);
    dht.begin();  // initialise sesnor
    //configure the esp32 with real time
    configTime(0,0,"pool.ntp.org","time.nist.gov");
    //configure the peripheral pins
    pinMode(BTN_PLUGIN, INPUT_PULLUP);
    pinMode(BTN_PLUGOUT, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    //connect board to wifi
    connectWiFi();
    //configure mqtt server
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);  //mqtt server address of thingsboard and port number
    mqtt.setBufferSize(512);
    mqtt.setCallback(mqttCallback);
    
    //connect the board to cloud
    connectMQTT();  //Token, device id
   
}

unsigned long now;
unsigned long last_print;

void loop()
{
    //it keeps listening to the incoming requests
    mqtt.loop();
    //push  vals every 5 sec
    now = millis();
    if((now - last_print) > 5000)
    {
        last_print = now;
        //read data from sensor  VOLTAGE, CURRENT, POWER, TEMPERATURE, BAY STATUS
          sampleSensor();
        // energy (Wh) = power (W) x throttle fraction x time (h); this block runs every 5 seconds
          if (bayStatus == "CHARGING") {
              energyWh += power * (throttleLevel / 100.0) * (5.0 / 3600.0);
          }
          //run ai to get prediction
         runEdgeAIInference();
        // overload protection always wins over a manual override
         if (bayStatus == "CHARGING" && current > overloadCurrentA) {
             manualOverrideActive = false;
         }
         //decide load based on the prediction
         if (!manualOverrideActive) {
             runOptimization();
         }
         
        //publish the data
         publishTelemetry();
         

    }
    plug_status();
    applyRelayDutyCycle();   //  switch the relay according to throttleLevel
    updateLeds() ;

    
}

