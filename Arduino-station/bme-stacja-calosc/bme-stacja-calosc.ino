/*********
  Complete project details at https://randomnerdtutorials.com  

  Server station
*********/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>


// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "ESP8266WebServer.h"


const char* ssid     = "dhtBMEserver";
const char* password = "dhtBMEserver#";

ESP8266WebServer server;
String getWeather();



/*#include <SPI.h>
#define BME_SCK 14
#define BME_MISO 12
#define BME_MOSI 13
#define BME_CS 15*/

#define DHTTYPE    DHT22 
#define DHTPIN D3

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;



int val = 0 ;


#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

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
