/***************************************************

Abhiraj Smart Home

ESP8266 + Sinric Pro

Relay : D1 (GPIO5)

Active LOW Relay
***************************************************/


#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#define WIFI_SSID       "HOME 2"
#define WIFI_PASS       "09876532"

#define APP_KEY         "689b1d39-c00f-49db-bdce-6e860e4b7d8a"

#define APP_SECRET      "e5051eab-102f-44ca-b2cc-a53d421f8280-0ff22603-41fc-4e81-ae14-cedd50e4b9f2"

#define DEVICE_ID       "6a421deb969af7ec245eb98d"

#define RELAY_PIN D1

bool relayState = false;

//==============================
// Relay Control
//==============================

bool onPowerState(const String &deviceId, bool &state) {

relayState = state;

if (state) {
digitalWrite(RELAY_PIN, LOW);   // Active LOW Relay
Serial.println("Light ON");
} else {
digitalWrite(RELAY_PIN, HIGH);
Serial.println("Light OFF");
}

return true;
}

//==============================
// WiFi
//==============================

void setupWiFi() {

Serial.print("Connecting WiFi");

WiFi.begin(WIFI_SSID, WIFI_PASS);

while (WiFi.status() != WL_CONNECTED) {

Serial.print(".");  

delay(500);

}

Serial.println();
Serial.println("WiFi Connected");
Serial.println(WiFi.localIP());

}//==============================
// Sinric Pro Setup
//==============================

void setupSinricPro() {

SinricProSwitch &mySwitch = SinricPro[DEVICE_ID];

mySwitch.onPowerState(onPowerState);

SinricPro.begin(APP_KEY, APP_SECRET);

SinricPro.restoreDeviceStates(true);

}

//==============================
// Setup
//==============================

void setup() {

Serial.begin(115200);

pinMode(RELAY_PIN, OUTPUT);

// Relay OFF during boot
digitalWrite(RELAY_PIN, HIGH);

setupWiFi();

setupSinricPro();

Serial.println();
Serial.println("======================");
Serial.println("Abhiraj Smart Home");
Serial.println("ESP8266 Ready");
Serial.println("======================");

}//==============================
// Loop
//==============================

void loop() {

// Reconnect WiFi
if (WiFi.status() != WL_CONNECTED) {

Serial.println("WiFi Lost... Reconnecting");  

WiFi.begin(WIFI_SSID, WIFI_PASS);  

while (WiFi.status() != WL_CONNECTED) {  

  delay(500);  
  Serial.print(".");  

}  

Serial.println();  
Serial.println("WiFi Reconnected");

}

// Handle Sinric Pro
SinricPro.handle();

}
