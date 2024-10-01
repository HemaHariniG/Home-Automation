#include "DHT.h"  
#include <WiFi.h> 
#include <WiFiClient.h> 
 
#define dhtPin 4 
#define trigPin 17 
#define echoPin 16 
#define ldrPin 22 
#define led 15 
#define fan 5 
#define DHTTYPE DHT11 
 
const char* ssid = "vivo";   
const char* password = "hemaharini"; 
WiFiServer server(80); 
 
DHT dht(dhtPin,DHTTYPE); 
 
void DHTsetup() 
{ 
Serial.println("DHTxx test!"); 
dht.begin(); 
} 
float getTemp() 
{  
 float temp = dht.readTemperature(); 
 Serial.print("Temperature: "); 
 Serial.print(temp); 
 Serial.println("C"); 
 return temp; 
} 
 
float LDRsetup() 
{ pinMode(ldrPin,INPUT); 
} 
float getLight(){ 
  int light = analogRead(ldrPin); 
  return light; 
} 
 
float UltrasonicSensorsetup(){ 
  pinMode(trigPin,OUTPUT); 
  pinMode(echoPin,INPUT); 
} 
float getdist(){ 
 digitalWrite(trigPin,LOW); 
vTaskDelay(1000); 
 
digitalWrite(trigPin,HIGH); 
vTaskDelay(1000); 
digitalWrite(trigPin,LOW); 
 
float time=pulseIn(echoPin,HIGH); 
float dist = time*0.034/2; 
float inch= dist*39.37; 
Serial.print("Distance in cm: "); 
Serial.println("dist"); 
Serial.print("Distance in inch: "); 
Serial.println("inch"); 
return inch; 
} 
 
void relaySetup(){ 
  pinMode(led,OUTPUT); 
  pinMode(fan,OUTPUT); 
} 
 
 void wifiSetup() 
{ 
  Serial.print("Connecting to "); 
  Serial.print(ssid); 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) 
  { 
    Automode(); 
    vTaskDelay(1000); 
    Serial.print("."); 
  } 
  Serial.println(""); 
  Serial.println("WiFi connected..!"); 
  Serial.print("Got IP: ");   
  Serial.println(WiFi.localIP()); 
   
  server.begin(); 
  Serial.println("HTTP server started"); 
} 
 
void Automode(){ 
  float temp = getTemp(); 
  float light = getLight(); 
  float inch = getdist(); 
 
  if(inch<=6) 
{ 
   if (light < 800) { 
      digitalWrite(led,HIGH); 
      } 
   else { 
      digitalWrite(led,LOW); 
      } 
   if (temp >= 20 ) { 
      digitalWrite(fan,LOW); 
      } 
   else{ 
      digitalWrite(fan, HIGH); 
      } 
} 
else { 
  digitalWrite(led, LOW); 
  digitalWrite(fan,LOW); 
   } 
} 
 
void setup() 
{ 
  Serial.begin(9600); 
  DHTsetup(); 
  LDRsetup(); 
  UltrasonicSensorsetup(); 
  relaySetup(); 
  wifiSetup(); 
} 
 
bool Manualmode = false; 
 
void loop() 
{ 
  WiFiClient client = server.available(); 
  if (client) 
  { 
    bool currentlineisBlank = true; 
    String buffer = ""; 
    while(client.connected()) 
    { 
      if (client.available()) 
      { 
        char c = client.read(); 
        buffer += c; 
        if (c == '\n' && currentlineisBlank) 
        { 
          client.println("HTTP/1.1 200 OK"); 
          client.println("Content-Type: text/html"); 
          client.println(); 
          client.println("<HTML><title>ESP32</title>"); 
          client.println("<body><h1>HOME AUTOMATION CONTROL</h1>"); 
          client.println("<body><h1>Choose your mode</h1>"); 
 
          client.println("<p>Manual Control</p>"); 
          client.println("<a href=\?manualOn\"\"><button>On</button></a>"); 
          client.println("<a href=\?manualOff\"\"><button>Off</button></a>"); 
 
          client.println("<p>Light</p>"); 
          client.println("<a href=\?lightOn\"\"><button>On</button></a>"); 
          client.println("<a href=\?lightOff\"\"><button>Off</button></a>"); 
           
          client.println("<p>Fan</p>"); 
          client.println("<a href=\?fanOn\"\"><button>On</button></a>"); 
          client.println("<a href=\?fanOff\"\"><button>Off</button></a>"); 
 
          client.println("</body></HTML>"); 
          break; 
        } 
         
        else if (c=='\n') 
        { 
          currentlineisBlank = true; 
          buffer = ""; 
        } 
        else if (c == '\r'); 
        { 
          if(buffer.indexOf("GET /?lightOff")>=0) 
           digitalWrite(led, LOW); 
          if(buffer.indexOf("GET /?lightOn")>=0) 
           digitalWrite(led,HIGH); 
 
          if(buffer.indexOf("GET /?fanOff")>=0) 
           digitalWrite(fan,LOW); 
          if(buffer.indexOf("GET /?fanOn")>=0) 
           digitalWrite(fan,HIGH); 
         
          if(buffer.indexOf("GET /?manualOn")>=0) 
          { 
           Manualmode = true; 
           Serial.print("Manualmode: "); 
           Serial.print(Manualmode); 
          } 
          if(buffer.indexOf("GET /?manualOff")>=0) 
          { 
           Manualmode = false; 
           Serial.print("Manualmode: "); 
           Serial.print(Manualmode); 
          } 
        } 
      } 
    } 
  } 
  else 
  { 
     bool currentlineisBlank = false; 
  } 
  client.stop(); 
  vTaskDelay(2000); 
  if(Manualmode == false) 
  { 
    Serial.println("Automode"); 
    Automode(); 
  } 
  else 
  { 
    Serial.println("Manualmode"); } }
