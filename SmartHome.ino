
/***************************************************
  Abhiraj Smart Home
  ESP8266 + SinricPro + IR Remote

  Relay : D1 (GPIO5)
  IR Receiver : D2

  NOTE:
  1. Upload once.
  2. Open Serial Monitor @115200.
  3. Press your remote button.
  4. Replace IR_BUTTON_CODE with the printed HEX value.
***************************************************/

#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <IRremote.hpp>

#define WIFI_SSID "HOME 2"
#define WIFI_PASS "09876532"

#define APP_KEY "689b1d39-c00f-49db-bdce-6e860e4b7d8a"
#define APP_SECRET "e5051eab-102f-44ca-b2cc-a53d421f8280-0ff22603-41fc-4e81-ae14-cedd50e4b9f2"
#define DEVICE_ID "6a421deb969af7ec245eb98d"

#define RELAY_PIN D1
#define IR_PIN D2

// Replace after first upload with your remote HEX code
uint32_t IR_BUTTON_CODE = 0xFF52AD; // Assumed '9' code (not guaranteed)

bool relayState = false;

void setRelay(bool state){
  relayState = state;
  digitalWrite(RELAY_PIN, state ? LOW : HIGH); // Active LOW relay
}

bool onPowerState(const String &deviceId, bool &state){
  setRelay(state);
  Serial.println(state ? "Light ON" : "Light OFF");
  return true;
}

void setupWiFi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setupSinric(){
  SinricProSwitch &sw = SinricPro[DEVICE_ID];
  sw.onPowerState(onPowerState);
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup(){
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  setupWiFi();
  setupSinric();

  Serial.println("Smart Home Ready");
}

void loop(){

  if(WiFi.status()!=WL_CONNECTED){
    WiFi.disconnect();
    setupWiFi();
  }

  SinricPro.handle();

  if(IrReceiver.decode()){

    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

    Serial.print("IR HEX: 0x");
    Serial.println(code, HEX);

    if(code == IR_BUTTON_CODE){

      setRelay(!relayState);

      SinricProSwitch &sw = SinricPro[DEVICE_ID];
      sw.sendPowerStateEvent(relayState);

      Serial.println(relayState ? "Relay ON" : "Relay OFF");
    }

    IrReceiver.resume();
  }
}
