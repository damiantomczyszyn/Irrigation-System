#include "decisionTree.h"
#include<cstdlib>


//mock up data
  float h = 10.1;   //humidity
  float t = 25.5;   //temperature
  int   w = 50;     //wather value

//values from  weather api mock up
  float temperature = 33.4;
  float pressure = 100; 
  float humidity = 30;
  float windSpeed = 30;
  //czas



  //decision tree params
  float czyJestMokro = 50;
  int iloscGodzinDoOpadow1 = 24;
  float iloscOpadowWLitrach1 = 4;
  float maksymalnaTemperatura1 = 25;
  ///----------------------
  
  int iloscGodzinDoOpadow2 = 24;
  float iloscOpadowWLitrach2 = 4;
  float maksymalnaTemperatura2 = 25;
  
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

bool makeWatheringDecision()
{
    if( w > czyJestMokro )
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