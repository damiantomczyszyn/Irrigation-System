#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "decisionTree.h"
#include <TimeLib.h>

#include "globals.h"
//---------------------------------------
#ifndef STASSID
#define STASSID "" // wifi name
#define STAPSK ""// wifi password
#endif
#define ZAW1 D0 //zawory
#define ZAW2 D1 //zawory
#define ZAW3 D2 //zawory
#define ZAW4 D3 //zawory
#define ILOSC_ZAWOROW 4

short dniBezPodlewania = 0;
const char *ssid = STASSID;
const char *password = STAPSK;
//---------------------------------------
ESP8266WebServer server(80); //serwer do przyjmowania requestów http
String dateAndTime=" ";//date and time init


WiFiClient client;

String wczoraj;
String jutro;
String apikey="";

int program=0;//aktualny program podlewania(0-nie wybrany)
unsigned long  lastWatheringTime = millis(); // sprawdz czy tyle milis się zmieści ile trzeba - czas od ostatniego podlewania
bool watheringIsOn=false; // czy podlewanie jest aktualnie właczone
unsigned long  watheringTimeOn= millis(); // czas podlewania dla danego zaworu
int podlewany=0;//zmienna informująca który zawór jest aktualnie podlewany
int  tabZaworow[ILOSC_ZAWOROW] = {
  ZAW1,ZAW2,ZAW3,ZAW4
};
constexpr int watherON = 0;// Stan włączenia podlewania
constexpr int watherOFF = 1;//stan wyłączenia podlewania
constexpr unsigned long watheringStartupTime = 79200000;//godzina 22 w ms
unsigned short czas[3] = {0,0,0};
unsigned long setupTime=0;
unsigned long czasCzekania;

//declare functions
String getTime();
void changeWathering();


unsigned long lastConnectionTime = 0;     // last time you connected to the server, in milliseconds
const unsigned long postInterval = 10 * 1000;  // posting interval of 10 minutes  (10L * 1000L; 10 seconds delay for testing)

//web api section
String getHtmlContent(const char* url);
String getDataFromStation();
//
const size_t capacity = JSON_OBJECT_SIZE(800) + 300;

const char*  serverApi = "weather.visualcrossing.com";  // Server URL
DynamicJsonDocument doc(capacity);
WiFiClientSecure clientS;
String jsonResponse = "";

DynamicJsonDocument doc2(150);  




void handleRoot() {

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
<button id=\"off\">Wyłącz</button><br>\
<button id=\"p1\">program1</button><br>\
<button id=\"p2\">program2</button><br>");

temp.println("Time:");
temp.println(getTime());
temp.println("<br>Date:");
temp.println(dateAndTime);
temp.println("<br>");
temp.printf("\
<script>\
    document.getElementById(\"on\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/on\");zapytanie.send();};\
    document.getElementById(\"off\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/off\");zapytanie.send();};\
</script>\
</body>\
</html>" );
  server.send(200, "text/html", temp.c_str());

}

void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) 
  { 
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; 
  }

  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 0);
}

void connectAndSave() {
  Serial.println("\nStarting connection to server...");
  clientS.setInsecure();//skip verification
  if (!clientS.connect(serverApi, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    clientS.println("GET https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/Lublin/" + wczoraj + "/" + jutro + "?unitGroup=metric&elements=datetime%2Ctemp%2Cdew%2Chumidity%2Cprecip%2Cprecipprob%2Cpreciptype%2Cwindspeedmax%2Cwindspeedmean%2Cpressure%2Cuvindex&include=fcst%2Cremote%2Cobs%2Cdays&key="+ apikey + "&contentType=json HTTP/1.0");
    clientS.println("Host: weather.visualcrossing.com");
    clientS.println("Connection: close");
    clientS.println();

    while (clientS.connected()) {
      String line = clientS.readStringUntil('\n');
      
      Serial.println(line); // HTTP/1.1 200
    }
    // if there are incoming bytes available
    // from the server, read them and print them:

    while (clientS.available()) {
      char c = clientS.read();
     // Serial.write(c);
      if(c=='{')
      {
        jsonResponse = "{";
        while (clientS.available())
        {
                char c = clientS.read();
           //     Serial.write(c);
          jsonResponse+=c;
        }
      }
    }
    Serial.println();
    clientS.stop();


  Serial.println(jsonResponse);
  //to json
    deserializeJson(doc, jsonResponse);
      //Serial.println("Estimated memory usage: " + String(measureJson(doc)) + " bytes");
    if (doc.isNull()) {
        Serial.println("Parsing input failed!");
        return;
      }
    
    Serial.println("JSON object = ");
    serializeJsonPretty(doc, Serial);
    Serial.println("Estimated memory usage: " + String(measureJson(doc)) + " bytes");
  }

}

void setup(void) 
{
  pinMode(LED_BUILTIN, OUTPUT);//ustawienie leda do świecenia jako output
  pinMode(ZAW1, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW2, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW3, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW4, OUTPUT);// wyjście dla zaworów

  //-----------------------------------
  digitalWrite(LED_BUILTIN, 0);//włączenie leda

  //-------------------------------------
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");// aby wszystkie przypadkowe znaki po otwarciu Seriala były oddzielone

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
  getTime();// zapisz czas
  connectAndSave();


  String jsonDataString = getDataFromStation();
  deserializeJson(doc2, jsonDataString);

  makeWatheringDecision(doc,doc2,dniBezPodlewania);//(weather api json, station json, dni bez włączenia podlewania)

  server.on("/", handleRoot);
  server.on("/on", [](){
        digitalWrite(LED_BUILTIN, LOW);                                      //zapal diodę
        digitalWrite(ZAW1, HIGH);   // pin13
        server.send(200);   });
  
  server.on("/off", []() {
      digitalWrite(LED_BUILTIN, HIGH);                                        //zgaś diodę
      digitalWrite(ZAW1, LOW); 
      server.send(200);   
  });

  

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");


 
  //wylicz ile trzeba poczekać do godziny 21 (zobacz czy sie spieszy godzine czy późni)
  setupTime = czas[0] *60*60*1000 + czas[1] *60*1000 + czas[2] *1000;// w ms
  //godzina 22 w ms to -> 79 200 000
  if(setupTime>watheringStartupTime)// jest już po czasie
  {
    czasCzekania = 86400000 - (setupTime-watheringStartupTime)+millis(); // odjąć czas nadwyżki od 24h w ms
  }
  else//jest przed czasem
  {
    czasCzekania=watheringStartupTime-setupTime+millis(); //dodanie milis startowo aby potem odejmować od aktualnego milis i porównać wartości
  }


    Serial.println("przed http requestem");
  if (millis() - lastConnectionTime > postInterval) {
    Serial.println("10 sekund");
    // note the time that the connection was made:
    lastConnectionTime = millis();
    
  }
    Serial.println("po http requestem");

}
//--------------------------loop loop
unsigned long oczekiwanie=0;
void loop(void) {

  server.handleClient();
  MDNS.update();
  delay(500);//czas na odpowiedź

  if(czasCzekania<=millis())//jeśli już czas to sprawdź czy odpalić test na podlewanie
{   
   getTime();//aktualizuj date i czas
  czasCzekania=86400000+millis();//24h w ms do odjecia jeszcze czas podlewania w przyszlosci #TODO
  
    //if(wathering()==true){
    if(makeWatheringDecision(doc,doc2)==true)// czy właczyć podlewanie czy nie z drzewa
    {
      
      digitalWrite(LED_BUILTIN, LOW);  //tylkko testowo zapal leda
      unsigned long  watheringTimeOn= millis(); //zapisz czas rozpoczęcia podlewania
      dniBezPodlewania=0;
    }
    else
    {
      dniBezPodlewania++;
    }
  if(watheringIsOn)
  changeWathering();
}

}

String getTime(){
const long utcOffsetInSeconds = 3600;

String daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);


  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }
  timeClient.begin();
  timeClient.update();



  //Serial.println(timeClient.getFormattedTime());
  Serial.println(timeClient.getEpochTime());
  Serial.println(timeClient.getDay());
  
  // Konwersja epoch time na strukturę czasu
  struct tm *timeinfo;
  time_t epoch_time = timeClient.getEpochTime();
  timeinfo = localtime(&epoch_time);
  // Uzyskanie formatowanej daty i czasu
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);// mogę sobie uciąć samą datę bez godziny do dziennego API
  Serial.println("Obecna data: " + String(buffer));
  dateAndTime = buffer;

  // Uzyskanie daty dnia następnego
  timeinfo->tm_mday += 1;
  mktime(timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  Serial.println("Data dnia następnego: " + String(buffer));
  jutro = String(buffer);

  // Uzyskanie daty dnia poprzedniego
  timeinfo->tm_mday -= 2; // Odejmujemy 2, aby uzyskać datę dnia poprzedniego
  mktime(timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
  Serial.println("Data dnia poprzedniego: " + String(buffer));
  wczoraj = String(buffer);
  czas[0]=timeClient.getHours();
  czas[1]=timeClient.getMinutes();
  czas[2]=timeClient.getSeconds();
  return (String)timeClient.getFormattedTime();

}

void changeWathering(){
   unsigned long now = millis();
  if(now - watheringTimeOn>10*1000*60)//10min
  {
  if(podlewany==ILOSC_ZAWOROW)//sprawdzay czy to jest ostatni zawor
    {
    podlewany=-1;//resetujemy na pierwszy zawór
    watheringIsOn=false; // wylaczamy flage podlewania
    digitalWrite(tabZaworow[podlewany], watherOFF);//włączenie podlewania zaworu
    return ;
  }
    podlewany++;//zaczynamy od inkrementu by było na 0 - czyli pierwszy elemeenty tablicy
    digitalWrite(tabZaworow[podlewany], watherON);//włączenie podlewania zaworu
    if(podlewany==0)
    return ;
    digitalWrite(tabZaworow[podlewany-1], watherOFF);//włączenie podlewania zaworu
    

  //zmiana zaworu na kolejny(wyłączenie aktualnego i włączenie następnego)

  }
  
}


String getDataFromStation(){
  //Your Domain name with URL path or IP address with path
String serverName = "http://192.168.101.76/json";
String dataJson = "";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;

unsigned long timerDelay = 5000;

  if ((millis() - lastTime) > timerDelay) {

    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      WiFiClient client; 
      String serverPath = serverName;

      http.begin(client, serverPath.c_str());
      

      int httpResponseCode = http.GET();
      Serial.println("wykonanie get do servera http");
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        dataJson = http.getString();

        
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  return dataJson;
}