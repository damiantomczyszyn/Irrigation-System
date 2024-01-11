#include "decisionTree.h"
#include <cstdlib>



//mock up sensors data
  float h = 10.1;   //humidity
  float t = 25.5;   //temperature
  int   w = 50;     //wather value

//mock up values from weather api 
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
    if( w > czyJestMokro )//1
    {// tak jest mokro
        probabilitySum += 0;

        if (czyPadaloWiecejNiz(0))//2
        {//tak padalo wiecej niz x litrow
            probabilitySum +=0;
        }
        else//2
        {//nie padalo wiecej niz x litrow
            probabilitySum +=35;
            
            if(czasOdOstatniegoPodlewaniaWiekszyNiz(0))//3
            {//tak wiekszy niz x godzin
                probabilitySum += 35;
            }
            else
            {// nie mniejszy niz x godzin
                probabilitySum += 5;
            }
        }
    }
    else//1
    {//nie jest mokro
     probabilitySum += 35;

        if ( czyBedziePadacWCiaguGodzin(0) )//2
        {//tak bedzie padac
        probabilitySum += -20;
       

        }
        else//2
        {//nie bedzie padac
            probabilitySum +=35;

            if(cieplejNizStopni(0))//3
            {//tak cieplej
                probabilitySum += -25;
            }
            else
            {// nie nie cieplej
                probabilitySum += 25;
            }    
        }
    }



    if( probabilitySum <= returnRandomInt() )// podjęcie decyzji losowo na podstawie prawdopodobieństwa
    {
        return true;
    }
    else
    {
    return false;
    }

}