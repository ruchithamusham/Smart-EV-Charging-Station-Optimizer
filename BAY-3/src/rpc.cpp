#include <ArduinoJson.h>
#include "rpc.h"
#include "network.h"
#include "state.h"
#include "config.h"

// ---------------------------------------------------------------------
// SRS 8.5: RPC command handling.
// ---------------------------------------------------------------------
void handleRpc(String requestId, char* payload) {
  JsonDocument doc;
  if (deserializeJson(doc, payload)) return;

  String method = doc["method"] | "";
  JsonObject params = doc["params"];

  JsonDocument response;

  if (method == "setRelayState") {
        bool state = false;
    JsonVariant p = doc["params"];
    if (p.is<bool>()) {
      state = p.as<bool>();          // dashboard sends plain true or false
    } else {
      state = p["state"] | false;    // {"state": true} format also works
    }
    if (state){
      digitalWrite(RELAY_PIN,  HIGH );

    }
    else{
      digitalWrite(RELAY_PIN,  LOW );
    }
    manualOverrideActive = true;
    throttleLevel = state ? 100 : 0;
    loadDecision = state ? "ALLOW" : "MANUAL_OFF";
    response["success"] = true;
    response["throttleLevel"] = throttleLevel;
    Serial.print(">> RPC setRelayState("); Serial.print(state); Serial.println(") - manual override engaged.");

  } else if (method == "setThrottle") {
    int level = params["level"] | 100;
    level = constrain(level, 0, 100);
    manualOverrideActive = true;
    throttleLevel = level;
    loadDecision = "MANUAL_THROTTLE";
    response["success"] = true;
    response["throttleLevel"] = throttleLevel;
    Serial.print(">> RPC setThrottle("); Serial.print(level); Serial.println(") - manual override engaged.");

  }else {
    response["success"] = false;
    response["error"] = "unknown method";
  }

  char buffer[220];
  serializeJson(response, buffer);
  mqtt.publish(("v1/devices/me/rpc/response/" + requestId).c_str(), buffer);
}
