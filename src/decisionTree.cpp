#include "decisionTree.h"
#include <cstdlib>
#include <ArduinoJson.h>
#include "globals.h"

short probabilitySum = 0;
DynamicJsonDocument* stationDoc = nullptr;
DynamicJsonDocument* weatherApiDoc = nullptr;



bool czyBedziePadacWCiaguNastepnegoDnia() 
{
    if((float)(*weatherApiDoc)["days"][2]["precipprob"] > szansaNaOpad)
        return true;
    return false;
}

bool czyJestMokro()
{
    if ((float)(*stationDoc)["WaterLewel"] > waterLewel )// m3/m3
        return true;
    return false;
}

bool czyPadaloDuzoPrzezOstatnieDwaDni()
{
    if((float)(*weatherApiDoc)["days"][1]["precip"] + (float)(*weatherApiDoc)["days"][0]["precip"] >  opadZDwóchDni)//2 dni opadow w mm
        return true;
    return false;
}

int returnRandomInt()
{
    return rand()%101; // random int 0 - 100
}

bool czasOdOstatniegoPodlewaniaWiekszyNizCzteryDni()
{
    if(dniBezPodlewania > iloscDniBezPodlewania)
        return true;
    return false;
}
bool srednioCieplejNiz(float stopnie = 20)
{
    if((float)(*weatherApiDoc)["days"][1]["temp"] > stopnie)
        return true;
    return false;
}

bool cieplejNiz25StopniCelc()
{
    if((float)(*stationDoc)["Temperature"] > 25.f)
        return true;
    return false;
}
bool czyJestWilgotno()
{
    if((float)(*stationDoc)["Humidity"] > 55.f)
        return true;
    return false;
}
bool dzisiejszyUVindeksWiekszyOd(int indeks=5)// domyślnie wiekszy od 5. Indeks 0-11
{
    if((float)(*weatherApiDoc)["days"][1]["uvindeks"] > indeks)
        return true;
    return false;
}

bool czyCisnienieJestMale()
{
    if((float)(*stationDoc)["Pressure"] < 1000.f)
        return true;
    return false;
}
bool spadnieWiecejNiz20MM()
{
    if((float)(*weatherApiDoc)["days"][2]["precip"] >  20.0f)//2 dni opadow w mm
        return true;
    return false;
}

bool makeWatheringDecision(DynamicJsonDocument doc, DynamicJsonDocument doc2)//(weather api json, station json, dni bez włączenia podlewania)
{   

    //dodaj wszędzie zabezpieczenie przed nullami
    //dodaj zliczenia dni bez podlewania. niech gdzieś to zwraca i trzyma w pamięci.
    //

    //0 -wczoraj
    //1 -dzisiaj
    //2 -jutro
    probabilitySum = 0;

   // Serial.println("\nDni bez podlewania w makeWathering");
   // Serial.println(dniBezPodlewania);
    // Serial.println("\nmake wathering decision doc");
    // serializeJsonPretty(doc["days"], Serial);
    // Serial.println("\nmake wathering decision doc2");
    // serializeJsonPretty(doc2, Serial);
    
    weatherApiDoc = &doc;
    stationDoc = &doc2;
    Serial.println("");
    float temp = (*weatherApiDoc)["days"][0]["temp"];
    // Serial.print("TEMPERATURKA W FLOAT: ");
    // Serial.println(temp);
    // Serial.println("json z wskaznika0");
    // serializeJson((*weatherApiDoc)["days"][0]["temp"], Serial);
    // Serial.println("json z wskaznika1");
    // serializeJson((*weatherApiDoc)["days"][1], Serial);
    // Serial.println("json z wskaznika2");
    // serializeJson((*weatherApiDoc)["days"][2], Serial);



    //-------------------------
    if(czyJestMokro())
    {// tak jest mokro
        probabilitySum += -20;

        if (czyPadaloDuzoPrzezOstatnieDwaDni())
        {//tak padalo wiecej niz x litrow
            probabilitySum += 0; // koniec suma 0
        }
        else
        {//nie padalo wiecej niz x litrow
            probabilitySum += 25;
            
            if(czasOdOstatniegoPodlewaniaWiekszyNizCzteryDni())
            {//tak wiekszy niz x godzin
                probabilitySum += 35;

                if(srednioCieplejNiz(srStopnie)){
                    probabilitySum += 25;
                   //koniec suma 85

                }
                else
                {
                    probabilitySum += -25;
                     if(czyJestWilgotno())
                    {
                        probabilitySum += 5; // koniec suma 40
                    }
                    else
                    {
                        probabilitySum += -3; //koniec suma 32
                    }
                }
            }
            else
            {// nie mniejszy niz x godzin
                probabilitySum += 5;
                if (srednioCieplejNiz(srStopnie))
                {
                    
                    probabilitySum += 25;

                    if(czyCisnienieJestMale())
                    {
                        probabilitySum += 5;//koniec 60
                    }
                    else
                    {
                        probabilitySum += -3;//koniec 52
                    }
                    
                }
                else
                {
                    probabilitySum += -25;
                    
                    if(dzisiejszyUVindeksWiekszyOd())
                    {
                        probabilitySum += 5;//koniec 10
                    }
                    else
                    {
                        probabilitySum += -3; //koniec 2
                    }
                }
            }
        }
    }
    else
    {//nie jest mokro
     probabilitySum += 35;

        if (czyBedziePadacWCiaguNastepnegoDnia() )
        {//tak bedzie padac
        probabilitySum += -10;

        if(spadnieWiecejNiz20MM())
        {
            probabilitySum += -15;

            if( dzisiejszyUVindeksWiekszyOd())
            {
                probabilitySum += 5; // koniec 15
            }
            else
            {
                probabilitySum += -3; //koniec 7
            }
        }
        else
        {
            probabilitySum +=15;

            if(srednioCieplejNiz(srStopnie))
            {
                probabilitySum += 25;
            
            if(dzisiejszyUVindeksWiekszyOd())
            {
                probabilitySum += 7;

                if (srednioCieplejNiz(srStopnie))
                {
                    probabilitySum += -25; // koniec 47
                }
                else
                {
                probabilitySum += 25; // koniec 97
                }
            }
            else
            {
                probabilitySum += -5;

                if (srednioCieplejNiz(srStopnie))
                {
                    probabilitySum += 25; // koniec 85
                    
                }
                else
                {
                probabilitySum += -25; // koniec 35
                }
            }
            }
           
            else
            {
                probabilitySum += -10; //koniec suma 30
            }

        }

        }
        else
        {//nie bedzie padac
            probabilitySum +=15;

            if(czasOdOstatniegoPodlewaniaWiekszyNizCzteryDni())
            {
                probabilitySum +=15;
                
                if(czyCisnienieJestMale())
                {
                    probabilitySum += 5;

                    if(srednioCieplejNiz(srStopnie))
                    {
                       probabilitySum += 10;//koniec 80
                    }
                    else
                    {
                        
                         probabilitySum += -35; //koniec 35
                    }
                }
                else
                {
                    probabilitySum += -3;
                    
                    if(srednioCieplejNiz(srStopnie))
                    {
                         probabilitySum += 10; //koniec 62
                        
                    }
                    else
                    {
                       probabilitySum += -35;// koniec 27
                    }
                }
            }
            else
            {
                probabilitySum += 5;

                if(czyCisnienieJestMale())
                {
                    probabilitySum +=5;

                    if(srednioCieplejNiz(srStopnie))
                    {
                        probabilitySum += 25;// koniec 85 
                       
                    }
                    else
                    {
                         probabilitySum += -25; // koniec 35
                    }
                }
                else
                {
                    probabilitySum += -3;

                    if(dzisiejszyUVindeksWiekszyOd())
                    {
                        probabilitySum += 7;

                        if(srednioCieplejNiz(srStopnie))
                        {
                            probabilitySum += 25;// koniec 84
                            
                        }
                        else
                        {
                            probabilitySum += -25; //koniec 34
                        }
                    }
                    else
                    {
                        probabilitySum += -5;

                         if(srednioCieplejNiz(1))
                        {
                            probabilitySum += 25;// koniec 84
                           
                        }
                        else
                        {
                             probabilitySum += -25; //koniec 34
                        }
                    }
                }
            }
        }
    }

    if( probabilitySum >= returnRandomInt() ) // losowanie na podstawie zebranej szansy włączenia podlewania lub nie
    {
        return true;
    }
    else
    {
    return false;
    }
}