#include "treeTester.h"
#include <cstdlib>
#include <ArduinoJson.h>
#include "decisionTree.h"
#include <WiFiClientSecure.h>
//można dodać zmienną klucz do api, aby łatwiej ją wpisywać a nie szukać w url

WiFiClientSecure clientS;
const char*  serverApi = "weather.visualcrossing.com";  // Server URL
String wczorajTest;
String jutroTest;

bool setVariables(WiFiClientSecure client)
{
    clientS = client;


    return true;
}


//connect to get json   //W pętli branie api na kolejny dzień i uruchamianie oraz zapisywanie wyniku gdzieś
void connectAndSave() {
  Serial.println("\nStarting connection to server...");
  clientS.setInsecure();//skip verification
  if (!clientS.connect(serverApi, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    clientS.println("GET https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/Lublin/" + wczorajTest + "/" + jutroTest + "?unitGroup=metric&elements=datetime%2Ctemp%2Cdew%2Chumidity%2Cprecip%2Cprecipprob%2Cpreciptype%2Cwindspeedmax%2Cwindspeedmean%2Cpressure%2Cuvindex&include=fcst%2Cremote%2Cobs%2Cdays&key=&contentType=json HTTP/1.0");
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


bool makeTestWatheringDecision(DynamicJsonDocument doc, DynamicJsonDocument doc2, short dniBezPodlewania)//(weather api json, station json, dni bez włączenia podlewania)
{   
// doc i doc2 będzie mockowany z doc skoro znane są szanse 
//bo nie wiem czy znajdę api prognozowe historyczne zamiast api histori pogody
//najlepiej znaleść oba i porównywać


}