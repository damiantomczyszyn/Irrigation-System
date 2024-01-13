#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ESP8266WebServer.h"

const char* ssid     = "";
const char* password = "";

ESP8266WebServer server;
String getWeather();
String getJsonWeather();

#define DHTTYPE    DHT22 
#define DHTPIN D3
#define SEALEVELPRESSURE_HPA (1013.25)

DHT dht(DHTPIN, DHTTYPE);

float t = 0.0;
float h = 0.0;
int val = 0 ;
unsigned long delayTime;


Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI




void handleNotFound() 
{
   server.send(404, "text/plain", "Not found");
}
 


void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));
  dht.begin();       
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();



// wifi part:
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

server.on("/",[](){server.send(200,"text/plain",getWeather());});
//server.on("/control",toggleLED);
server.on("/json",[](){server.send(200,"text/plain",getJsonWeather());});
//server.on("/control",toggleLED);
  
  server.begin();
}
  


void loop() { 
  printValues();
  //delay(delayTime);
  server.handleClient();
}

void printValues() {
 
Serial.println(getWeather());  
Serial.println("**************************************************");
Serial.println();


 
}

String getWeather(){
  String weatherS="";
  weatherS+="Temperature = ";
  weatherS+=bme.readTemperature();
  weatherS+=" *C";
    
  weatherS+="Pressure = ";
  weatherS+=bme.readPressure() / 100.0F;
  weatherS+=" hPa";

  weatherS+="Approx. Altitude = ";
  weatherS+=bme.readAltitude(SEALEVELPRESSURE_HPA);
  weatherS+=" m";

  weatherS+="Humidity = ";
  weatherS+=bme.readHumidity();
  weatherS+=" %";

  weatherS+="\n";

  val=analogRead(A0); // Water Level Sensor output pin connected A0
  weatherS+="water lewel = ";  // See the Value In Serial Monitor
  weatherS+=val;  // See the Value In Serial Monitor
  weatherS+="\n";


h = dht.readHumidity();
  weatherS+="Humidity = ";
if (isnan(h)) {
   weatherS+="err";
}
else {
   weatherS+=h;
}
t = dht.readTemperature();
weatherS+="temperature = ";
if (isnan(t)) {
   weatherS+="err";
}
else {
  weatherS+=t;
}

return weatherS;
}

String getJsonWeather(){
  String weatherS = "{";

//const char* jsonString = R"({"Temperature": 25.26,"Pressure": 1000.50,"Altitude": 106.70,"Humidity": 28.79})";

  weatherS+="\"Temperature\": ";
  weatherS+=bme.readTemperature();
  weatherS+=",";
    
  weatherS+="\"Pressure\": ";
  weatherS+=bme.readPressure() / 100.0F;
  weatherS+=",";

  weatherS+="\"Altitude\": ";
  weatherS+=bme.readAltitude(SEALEVELPRESSURE_HPA);
  weatherS+=",";

  weatherS+="\"Humidity\": ";
  weatherS+=bme.readHumidity();
  weatherS+=",";

  val=analogRead(A0); // Water Level Sensor output pin connected A0
  weatherS+="\"WaterLewel\": ";  // See the Value In Serial Monitor
  weatherS+=val;  // See the Value In Serial Monitor
  weatherS+="}";

  return weatherS;
}
