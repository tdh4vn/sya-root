#include "ESP8266.h"
#include <SPI.h>
#include <SD.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>
#include <EEPROM.h>

#define SSID        "^_^ <3 -_- <3 :3"
#define PASSWORD    "hanhphuclakhianhcoem"

#define WIFI_CONNECTED 2
#define DEVICE_STARTED 3
#define WEB_SERVER_STARTED 4

//#define SSID        "MrAndMiss"
//#define PASSWORD    "12345678"

int pinCS = 53;
File myFile;
int status = WL_IDLE_STATUS;

byte ledPin[] = {2, 3, 4};


ESP8266 wifi(Serial1);

WiFiEspClient espClient;

PubSubClient client(espClient);

void setupLed(){
  for(byte i = 0; i < 3; i++){
      pinMode(ledPin[i], OUTPUT);
  }
}

void turnOnLed(int LED_PIN){
  digitalWrite(LED_PIN, HIGH);
}

void turnOffLed(int LED_PIN){
  digitalWrite(LED_PIN, LOW);
}


void setupEsp8266(){
  Serial.begin(115200);   
  Serial1.begin(115200);
  Serial.print("Setup begin\r\n");
  
  WiFi.init(&Serial1);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  //connect wifi
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(SSID, PASSWORD);
  }
  turnOnLed(2);

  status = WL_IDLE_STATUS;
  //start AP
  while ( status != WL_CONNECTED) {
    Serial.print("Starting AP: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network
    status = WiFi.beginAP("ROOT_SYA", 6, "12345678", ENC_TYPE_WPA2_PSK, false);
  }
  turnOnLed(4);
  
  Serial.print("IP");
  Serial.println(WiFi.localIP());

}

void setupTCPServer(){
  if (wifi.enableMUX()) {
     Serial.print("multiple ok\r\n");
  } else {
     Serial.print("multiple err\r\n");
  }
  
  if (wifi.startTCPServer(8090)) {
     Serial.print("start tcp server ok\r\n");
  } else {
     Serial.print("start tcp server err\r\n");
  }
  
  if (wifi.setTCPServerTimeout(20)) { 
     Serial.print("set tcp server timout 20 seconds\r\n");
  } else {
     Serial.print("set tcp server timout err\r\n");
  }
}

void testFile(){
  pinMode(pinCS, OUTPUT);
  
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  // Create/Open file 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
    myFile.println("Testing text 1, 2 ,3...");
    myFile.close(); // close the file
    Serial.println("Done.");
  }else {
    Serial.println("error opening test.txt");
  }
  // Reading the file
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error opening test.txt");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup(void)
{
  setupLed();
  setupEsp8266();
  setupTCPServer();
  client.setServer("0.tcp.ngrok.io", 14838);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("ROOT_01")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("ROOT_01_CONTROL_EVENTS");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void receiveMessageFromNode()
{
    uint8_t buffer[128] = {0};
    uint8_t mux_id;
    uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
    if (len > 0) {
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
        
        Serial.print("Received from :");
        Serial.print(mux_id);
        Serial.print("[");
        char message[len];
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
            message[i]=(char)buffer[i];
        }
        
//        if(client.connected()){
//          client.publish("ROOT_01", message);
//        }
        Serial.print("]\r\n");
        
        if(wifi.send(mux_id, buffer, len)) {
            Serial.print("send back ok\r\n");
        } else {
            Serial.print("send back err\r\n");
        }
    }
}


void loop(void)
{
  if (!client.connected()) {
    //reconnect();
  }
  //client.loop();
  receiveMessageFromNode();
}



//lib: https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
