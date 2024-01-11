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
#include "decisionTree.h"

//---------------------------------------
#define DHTPIN 12   
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#ifndef STASSID
#define STASSID "dlink" // wifi name
#define STAPSK "tomczyszyn"// wifi password
#endif
#define SIGNAL_PIN A0 //wather sensor data pin
#define ZAW1 D0 //zawory
#define ZAW2 D1 //zawory
#define ZAW3 D2 //zawory
#define ZAW4 D3 //zawory
#define ILOSC_ZAWOROW 4
#define WATHER_SENSOR_UP 0 //stan dla którego czujnik wody wykrył wodę

const char *ssid = STASSID;
const char *password = STAPSK;
//---------------------------------------
ESP8266WebServer server(80); //serwwer do przyjmowania requestów
int watherValue = 0; // variable to store the sensor wather value
String dateAndTime=" ";//date and time init
DHT dht(DHTPIN, DHTTYPE);//stworzenie obiektu DHT dla wybranych pinów
WiFiClient client;
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
void parseJson(const char * jsonString);
void makehttpRequest();
bool wathering();
void changeWathering();
//

//weather api
//https://randomnerdtutorials.com/esp8266-weather-forecaster/

const char server1[] = "api.openweathermap.org";
String serverPath="";
String text;
int jsonend = 0;
boolean startJson = false;
int status = WL_IDLE_STATUS;
#define JSON_BUFF_DIMENSION 2500
unsigned long lastConnectionTime = 0;     // last time you connected to the server, in milliseconds
const unsigned long postInterval = 10 * 1000;  // posting interval of 10 minutes  (10L * 1000L; 10 seconds delay for testing)


 





void handleRoot() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
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
<button id=\"off\">Wyłącz</button><br>\
<button id=\"p1\">program1</button><br>\
<button id=\"p2\">program2</button>");
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
    document.getElementById(\"p1\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/codziennie\");zapytanie.send();};\
    document.getElementById(\"p2\").onclick = function () {const zapytanie = new XMLHttpRequest();zapytanie.open(\"GET\", \"/co2dni\");zapytanie.send();};\
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
  pinMode(ZAW1, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW2, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW3, OUTPUT);// wyjście dla zaworów
  pinMode(ZAW4, OUTPUT);// wyjście dla zaworów

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
        digitalWrite(ZAW1, HIGH);   // pin13
        server.send(200);   });
  
  server.on("/off", []() {
      digitalWrite(LED_BUILTIN, HIGH);                                        //zgaś diodę
      digitalWrite(ZAW1, LOW); 
      server.send(200);   
  });


  server.on("/codziennie", [](){
        
      program=1;
        server.send(200);   });
  
  server.on("/co2dni", []() {

      program=2;
      server.send(200);   
  });
  

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");


  getTime();// zapisz czas
  //wylicz ile trzeba poczekać do godziny 21 (zobacz czy sie spieszy godzine czy późni)
  setupTime=czas[0]*60*60*1000+czas[1]*60*1000+czas[2]*1000;// w ms
  //godzina 22 w ms to -> 79 200 000
  if(setupTime>watheringStartupTime)// jest już po czasie
  {
    czasCzekania= 86400000 - (setupTime-watheringStartupTime)+millis(); // odjąć czas nadwyżki od 24h w ms
  }
  else//jest przed czasem
  {
    czasCzekania=watheringStartupTime-setupTime+millis(); //dodanie milis startowo aby potem odejmować od aktualnego milis i porównać wartości
  }


makeWatheringDecision();
}
//--------------------------loop loop
unsigned long oczekiwanie=0;
void loop(void) {

  
  
  server.handleClient();
  MDNS.update();
  delay(500);//czas na odpowiedź

  if(czasCzekania<=millis())//jeśli już czas to sprawdź czy odpalić test na podlewanie
{   czasCzekania=86400000+millis();//24h w ms do odjecia jeszcze czas podlewania w przyszlosci #TODO
    if(wathering()==true){
      digitalWrite(LED_BUILTIN, LOW);  //tylkko testowo zapal leda
      unsigned long  watheringTimeOn= millis(); //zapisz czas rozpoczęcia podlewania
    }
  if(watheringIsOn)
  changeWathering();
}

//  // Serial.println("przed http requestem");
//   if (millis() - lastConnectionTime > postInterval) {
//     Serial.println("10 sekund");
//     // note the time that the connection was made:
//     lastConnectionTime = millis();
//     makehttpRequest();
//   }
//  //  Serial.println("po http requestem");

}



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

  // Serial.print(daysOfTheWeek[timeClient.getDay()]);
  // Serial.print(", ");
  // Serial.print(timeClient.getHours());
  // Serial.print(":");
  // Serial.print(timeClient.getMinutes());
  // Serial.print(":");
  // Serial.println(timeClient.getSeconds());

  //Serial.println(timeClient.getFormattedTime());
  dateAndTime=daysOfTheWeek[timeClient.getDay()]+timeClient.getHours()+timeClient.getMinutes()+timeClient.getSeconds();
  czas[0]=timeClient.getHours();
  czas[1]=timeClient.getMinutes();
  czas[2]=timeClient.getSeconds();
  return (String)timeClient.getFormattedTime();

}

int difTime(){
  return 0;
}

// to request data from OWM
void makehttpRequest() {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server1, 80)) {

     Serial.println("connecting...");
    // send the HTTP PUT request:

    client.println("GET " + serverPath);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: Mozilla/5.0");
    //client.println("Connection: close");
    client.println();

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    
    char c = 0;
    while (client.available()) {
      //Serial.println("odczytuje json");
      c = client.read();
      delay(500);// dodaniee delaya który ma pomóc w odczytaniu całości zanim instrukcje pójdą dalej
      // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
      // the open and close occurences,
      Serial.print(c);
      if (c == '{') {
        startJson = true;         // set startJson true to indicate json message has started
        jsonend++;
      }
      if (c == '}') {
        jsonend--;
      }
      if (startJson == true) {
        text += c;
      }
      // if jsonend = 0 then we have have received equal number of curly braces 
      if (jsonend == 0 && startJson == true) {
        Serial.println("json odczytany");
        parseJson(text.c_str());  // parse c string text in parseJson function


        text = "";                // clear text string for the next time
        startJson = false;        // set startJson to false to indicate that a new message has not yet started
      }
    }
  }
  else {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
}





//to parse json data recieved from OWM
void parseJson(const char * jsonString) {
  Serial.println("parsowanie");
  //StaticJsonBuffer<4000> jsonBuffer;
  const size_t bufferSize = 2*JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2*JSON_OBJECT_SIZE(7) + 2*JSON_OBJECT_SIZE(8) + 720;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // FIND FIELDS IN JSON TREE
  JsonObject& root = jsonBuffer.parseObject(jsonString);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  JsonArray& list = root["list"];
  JsonObject& nowT = list[0];


  // including temperature and humidity for those who may wish to hack it in
  
  String city = root["city"]["name"];
  float tempNow = nowT["main"]["temp"];
  float humidityNow = nowT["main"]["humidity"];
  String weatherNow = nowT["weather"][0]["description"];


  // checking for four main weather possibilities
  Serial.println("cokolwiek wypisuję");
  Serial.println();
  Serial.print("tempNow: ");
  Serial.println(tempNow);
  Serial.print("humidityNow: ");
  Serial.println(humidityNow);
  Serial.println();
}



bool wathering(){
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  unsigned long now = millis();
  watherValue = analogRead(SIGNAL_PIN); // read the analog value from sensor

  if(watherValue == WATHER_SENSOR_UP)// jeśli jest mokro to nie podlewaj
  {
    return false;
  }

  if(program==0)
  {
  if (now - lastWatheringTime>1*1000*60)//wiecej niż minute
  {
    lastWatheringTime=millis();
    watheringIsOn=true;
    return true;
    
  }
  watheringIsOn=false;
  return false;
  }
  else if (program==1)
  {
  if (now - lastWatheringTime>24*1000*60*60)//wiecej niż 24h
  {
    lastWatheringTime=millis();
    watheringIsOn=true;
    return true;
    
  }
    watheringIsOn=false;
  return false;
  }
  else if (program==2)
  {
  if (now - lastWatheringTime>48*1000*60*60)//wiecej 48h
  {
    lastWatheringTime=millis();
    watheringIsOn=true;
    return true;
    
  }
  watheringIsOn=false;
  return false;
  }
  else{
  watheringIsOn=false;
  return false;
  
  }
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
