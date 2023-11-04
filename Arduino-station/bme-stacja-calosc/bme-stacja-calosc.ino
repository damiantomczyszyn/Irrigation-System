/*********
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
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
}

void loop() { 
  printValues();
  delay(delayTime);
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();


  val=analogRead(A0); // Water Level Sensor output pin connected A0
  Serial.print("water lewel = ");  // See the Value In Serial Monitor
  Serial.println(val);  // See the Value In Serial Monitor
  Serial.println();





// Read Humidity
h = dht.readHumidity();
// if humidity read failed, don't change h value 
if (isnan(h)) {
   Serial.println("Failed to read from DHT sensor!");
}
else {
   Serial.print("Humidity dht = ");  // See the Value In Serial Monitor
  Serial.println(h);
}


t = dht.readTemperature();
// if humidity read failed, don't change h value 
if (isnan(t)) {
   Serial.println("Failed to read from DHT sensor!");
}
else {
  Serial.print("temperature dht = ");  // See the Value In Serial Monitor
  Serial.println(t);
}
Serial.println("**************************************************");
Serial.println();



  

  
}
