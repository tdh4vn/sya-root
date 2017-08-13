#include "RootWiFi.h"


char* RootWiFi::getSSID(){
  return this->ssid;
}
char* RootWiFi::getPassword(){
  return this->password;
}
void RootWiFi::setSSID(char *ssid, char* password){
  this->ssid = ssid;
  this->password = password;
}

RootWiFi RootWiFi::RootWiFi(){
  WiFi.init(&Serial1);

  int status = WL_IDLE_STATUS;

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  //connect wifi
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(this->ssid);
    status = WiFi.begin(this->ssid, this->password);
  }

  status = WL_IDLE_STATUS;
  //start AP
  while ( status != WL_CONNECTED) {
    Serial.print("Starting AP: ");
    Serial.println(SSID);
    status = WiFi.beginAP(this->apName, 6, this->apPass, ENC_TYPE_WPA2_PSK, false);
  }
}


