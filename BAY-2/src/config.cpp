#include "config.h"
// SETUP: this project needs include/secrets.h, which is NOT in the repo (it holds a private token).
// Create include/secrets.h in this bay with this one line:
//   #define SECRET_TB_TOKEN "your-thingsboard-device-token"
#include "secrets.h"

// ---------------- WiFi Details ----------------

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// ---------------- ThingsBoard Details ----------------

const char* MQTT_SERVER = "mqtt.thingsboard.cloud";
const int MQTT_PORT = 1883;

const char* TB_TOKEN = SECRET_TB_TOKEN;
const char* BAY_ID = "BAY2";