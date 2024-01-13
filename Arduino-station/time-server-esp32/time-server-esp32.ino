#include <WiFi.h>
#include "time.h"

const char* ssid     = "dlink";
const char* password = "tomczyszyn";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; // poland time zone 1-> 1*1h w sekundach https://www.timeanddate.com/time/map/
const int   daylightOffset_sec = 3600;


//------------------------Time Features
unsigned long timeInSeconds = 0;
unsigned long unixTimeInSeconds = 0; // https://www.unixtimestamp.com // 0 to 4,294,967,295 - unsigned long TODO zmiana na string
void saveTime();
unsigned long getTime();
unsigned long getHourTimeInSec();
unsigned long synchronizeTimeToHourInSec(int sHour);
void saveLocalTime();

struct tm savedTime;





//------------------------End Time Features

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop(){
  delay(1000);
  printLocalTime();
  saveLocalTime();
  Serial.print("czas oczekiwania do wlaczenia w s: ");
  Serial.println(synchronizeTimeToHourInSec(1));
  
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H"); // pozni sie godzine
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
  time_t now;
  time(&now);
  Serial.print("Epoch Time: ");
  Serial.println(now);
  Serial.print("Sekundy: ");
Serial.println(timeinfo.tm_sec);

Serial.print("Minuty: ");
Serial.println(timeinfo.tm_min);

Serial.print("Godzina: ");
Serial.println(timeinfo.tm_hour);


}

void saveLocalTime()
{
  
  if(!getLocalTime(&savedTime)){
    Serial.println("Failed to obtain time");
    return;
  }

  }

unsigned long getHourTimeInSec(){


   unsigned long currentHour = savedTime.tm_hour;
   unsigned long currentMin = savedTime.tm_min;
   unsigned long currentSec = savedTime.tm_sec;

  return currentHour*3600 + currentMin*60 + currentSec;

}

unsigned long synchronizeTimeToHourInSec(int sHour)
{
  unsigned long godzinaWSekundach = sHour * 3600;
  unsigned long aktualnyCzasWSekundach = getHourTimeInSec();
  unsigned long czasCzekania = 0;
  unsigned long roznicaCzasu = 0;

  if(aktualnyCzasWSekundach>=godzinaWSekundach) {
     //Serial.println("opcja1");
     roznicaCzasu = aktualnyCzasWSekundach - godzinaWSekundach;
     czasCzekania = 86400 - roznicaCzasu;
     return czasCzekania;
  }
  else
  {
    //Serial.println("opcja2");
     roznicaCzasu =  godzinaWSekundach - aktualnyCzasWSekundach;
     czasCzekania = roznicaCzasu;
     return czasCzekania;
  }
    
 // return 0;
}
