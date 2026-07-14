
/****************************************************
 Abhiraj Smart Home V2
 ESP8266 + SinricPro + IR Remote
 Relay: D1 (Active LOW)
 IR RX: D2
****************************************************/

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

bool relayState=false;
unsigned long lastIR=0;

void setRelay(bool s){
  relayState=s;
  digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
  Serial.print("Relay: ");
  Serial.println(relayState?"ON":"OFF");
}

bool onPowerState(const String&, bool &state){
  setRelay(state);
  return true;
}

void setupWiFi(){
  Serial.print("Connecting WiFi");
  WiFi.begin(WIFI_SSID,WIFI_PASS);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  pinMode(RELAY_PIN,OUTPUT);
  setRelay(false);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");

  setupWiFi();

  SinricProSwitch &sw=SinricPro[DEVICE_ID];
  sw.onPowerState(onPowerState);
  SinricPro.begin(APP_KEY,APP_SECRET);
  SinricPro.restoreDeviceStates(true);

  Serial.println("Smart Home Ready");
}

void loop(){

  if(WiFi.status()!=WL_CONNECTED){
    Serial.println("WiFi Reconnecting...");
    setupWiFi();
  }

  SinricPro.handle();

  if(IrReceiver.decode()){

    uint32_t code=IrReceiver.decodedIRData.decodedRawData;

    if(code!=0 && millis()-lastIR>250){

      lastIR=millis();

      Serial.print("IR HEX: 0x");
      Serial.println(code,HEX);

      setRelay(!relayState);

      SinricProSwitch &sw=SinricPro[DEVICE_ID];
      sw.sendPowerStateEvent(relayState);
    }

    IrReceiver.resume();
  }
}
