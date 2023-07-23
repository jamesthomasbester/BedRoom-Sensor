/* This demo shows how to display the CCS811 readings on an Adafruit I2C OLED. 
 * (We used a Feather + OLED FeatherWing)
 */
 
#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "OPTUS_69B7D1M";  
const char* password = "repel52562xf";  

WebServer server(80);


Adafruit_CCS811 ccs;
#define DHTTYPE DHT22
#define DHTPIN 19
DHT dht(DHTPIN, DHTTYPE);

void setup() {  
  Serial.begin(115200);

  // Delay for server init
  delay(10);

  //WiFi Server Init
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
 
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.on("/api", handle_API);
 
  server.begin();
  Serial.println("HTTP server started");

  //Air Quality Sensor
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
  while(!ccs.available());
  //Temp Sensor
  dht.begin();
  delay(500);
}

class Data{
  public:
    float eCO2 = ccs.geteCO2();
    float TVOC = ccs.getTVOC();
    float Temperature = dht.readTemperature(); 
    float Humidity = dht.readHumidity(); 
};

void loop() {
  server.handleClient();;
}



void handle_OnConnect() {
  if(ccs.available()){
    if(!ccs.readData()){
      Data data;
      server.send(200, "text/html", SendHTML(data.Temperature,data.Humidity, data.eCO2, data.TVOC)); 
    }
    else{
      Serial.println("Error!");
      while(1);
    }
  }
}

void handle_API(){
    if(ccs.available()){
    if(!ccs.readData()){
      char result[64];
      Data data;
      String ptr = "{\n";
      ptr += "\"temperature\": ";
      ptr += (int)data.Temperature;
      ptr += ",\n";
      ptr += "\"humidity\": ";
      ptr += (int)data.Humidity;
      ptr += ",\n";
      ptr += "\"co2\": ";
      ptr += (int)data.eCO2;
      ptr += ",\n";
      ptr += "\"TVOC\": ";
      ptr += (int)data.TVOC;
      ptr += "\n}";

      server.send(200, "application/json", ptr); 
    }
    else{
      Serial.println("Error!");
      while(1);
    }
  }
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

 
String SendHTML(float Temperaturestat,float Humiditystat, float eCO2stat, float TVOCstat)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Bedroom Sensor</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Bedroom Sensor</h1>\n";
  
  ptr +="<p>Temperature: ";
  ptr +=(int)Temperaturestat;
  ptr +=" C</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  ptr +="<p>CO2: ";
  ptr +=(int)eCO2stat;
  ptr +="ppm</p>";
  ptr +="<p>TVOC: ";
  ptr +=(int)TVOCstat;
  ptr +="ppb</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}