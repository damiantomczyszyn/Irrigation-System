#include "decisionTree.h"
#include<cstdlib>
#include <ArduinoJson.h>


short probabilitySum = 0;


bool czyBedziePadacWCiaguGodzin(int x) 
{
    return true;
}

bool czyPadaloWiecejNiz(float x)
{
    return true;
}

int returnRandomInt()
{
    return rand()%101; // random int 0 - 100
}

bool czasOdOstatniegoPodlewaniaWiekszyNiz(int godzin)
{
    return true;
}
bool cieplejNizStopni(float stopnie)
{
    return true;
}

bool makeWatheringDecision(DynamicJsonDocument doc, DynamicJsonDocument doc2)
{
    Serial.println("\nmake wathering decision doc");
    serializeJsonPretty(doc, Serial);
    serializeJsonPretty(doc2, Serial);




    //-------------------------
    if( 1==1 )
    {// tak jest mokro
        probabilitySum += 0;

        if (czyPadaloWiecejNiz(0))
        {//tak padalo wiecej niz x litrow
            probabilitySum +=0;
        }
        else
        {//nie padalo wiecej niz x litrow
            probabilitySum +=35;
            
            if(czasOdOstatniegoPodlewaniaWiekszyNiz(0))
            {//tak wiekszy niz x godzin
                probabilitySum += 35;
            }
            else
            {// nie mniejszy niz x godzin
                probabilitySum += 5;
            }
        }
    }
    else
    {//nie jest mokro
     probabilitySum += 35;

        if ( czyBedziePadacWCiaguGodzin(0) )
        {//tak bedzie padac
        probabilitySum += -20;
       

        }
        else
        {//nie bedzie padac
            probabilitySum +=35;

            if(cieplejNizStopni(0))
            {//tak cieplej
                probabilitySum += -25;
            }
            else
            {// nie nie cieplej
                probabilitySum += 25;
            }    
        }
    }

    if( probabilitySum <= returnRandomInt() )
    {
        return true;
    }
    else
    {
    return false;
    }
}