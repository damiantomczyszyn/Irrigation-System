#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <DHT.h>//biblioteka do DHT


//---------------------------------------
#define DHTPIN D6   
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#ifndef STASSID
#define STASSID "" // wifi name
#define STAPSK ""// wifi password
#endif
#define SIGNAL_PIN A0 //wather sensor data pin
#define WATHER_SENSOR_UP 0 //stan dla którego czujnik wody wykrył wodę

const char *ssid = STASSID;
const char *password = STAPSK;
//---------------------------------------
ESP8266WebServer server(80); //serwwer do przyjmowania requestów
int watherValue = 0; // variable to store the sensor wather value

DHT dht(DHTPIN, DHTTYPE);//stworzenie obiektu DHT dla wybranych pinów
WiFiClient client;

String getTime(){
const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);


  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }
  timeClient.begin();
  timeClient.update();

  return (String)timeClient.getFormattedTime();

}


void handleRoot() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  delay(100);
  watherValue = analogRead(SIGNAL_PIN); // read the analog value from sensor


  StreamString temp;
  temp.reserve(500);  // Preallocate a large chunk to avoid memory fragmentation
  temp.printf("\
<html>\
<head>\
    <title>Strona</title>\
    <meta charset=\"UTF-8\"/>\
  <!--  <meta http-equiv=\"refresh\" content=\"1\"> -->  <!-- przeladowywanie strony co x sekund --> \
</head>\
<body>\
<p id=\"pomiar\">Wartość:</p>\
<p id=\"default\">1</p>\
<button id=\"on\">Włącz</button>\
<button id=\"off\">Wyłącz</button><br>");
temp.print("<p>Sensor Value =  ");
  if (isnan(t) || isnan(h)) {
    temp.println("Failed to read from DHT");
  } else {
    temp.print("Humidity: "); 
    temp.print(h);
    temp.print(" %\t");
    temp.print("Temperature: "); 
    temp.print(t);
    temp.println(" *C");
  }
temp.println("</p>");
temp.println("Time:");
temp.println(getTime());

temp.print("<p>Wather sensor Value =  ");

  temp.println(watherValue);

temp.println("</p>");

temp.printf("\
<script>\
    document.getElementById(\"on\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/on\");zapytanie.send();};\
    document.getElementById(\"off\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/off\");zapytanie.send();};\
</script>\
</body>\
</html>" );
  server.send(200, "text/html", temp.c_str());

}

void handleNotFound() {//http request not handle
  digitalWrite(LED_BUILTIN, 1);//zgaś leda
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }

  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 0);
}

// SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP
void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);//ustawienie leda do świecenia jako output


  //-----------------------------------
  digitalWrite(LED_BUILTIN, 0);//włączenie leda

  //-------------------------------------
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  //dht.begin();


  // Wait for connection
  Serial.print("Connecting to wifi: ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


//wypisanie adresu
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());




  if (MDNS.begin("esp8266")) { 
    Serial.println("MDNS responder started"); 
    }

  server.on("/", handleRoot);

  server.on("/on", [](){
        digitalWrite(LED_BUILTIN, LOW);                                      //zapal diodę

        server.send(200);   });
  
  server.on("/off", []() {
      digitalWrite(LED_BUILTIN, HIGH);                                        //zgaś diodę

      server.send(200);   
  });


  

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}
void loop(void) 
{

  
  
  server.handleClient();
  MDNS.update();
  delay(500);//czas na odpowiedź




}




