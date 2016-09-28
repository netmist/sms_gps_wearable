/*
  From SMS to GPS wearable POC
  Version 0.1
*/

#include <LCheckSIM.h>
#include <LGSM.h>
#include <LGPS.h>
#include <LBattery.h>
#include <stdio.h>
#include <string.h>

//Indicators
int batteryLlevel;

//Flags
boolean newInSMS;
boolean failOutSMS;

//Buffers and vars
char rnum[20] = {0};
char buf_contex[100];
char buffer[140] = {0,};

void setup() {

  newInSMS = false;
  failOutSMS = false;
  Serial.begin(115200);
  while(!Serial.available()); 
  Serial.print("From SMS to GPS Wearable POC.\r\n");
  checkBattery();
  waitForGPS();
  Serial.flush();
}

void loop() {

  checkBattery();

  if(LSMS.available()||newInSMS)   // new incoming SMS
  {
    if(!newInSMS){
      LSMS.remoteNumber(rnum, 20); 
      LSMS.remoteContent(buf_contex, 50);
    }
          
    sprintf(buffer, "Get new sms, content: %s, number: %s \r\n", buf_contex, rnum);
    Serial.println(buffer);
          
    if((strcmp("GPS",(char*)buf_contex) == 0)||(newInSMS))
    {
      if(newInSMS){
        if(failOutSMS){
          Serial.println("Retrying to send SMS.");
        }else{
          Serial.println("Retrying after GPS lost.");
        }
      }else{
        Serial.println("GPS in SMS content, going ahead!");
        newInSMS = true;
      }    

      if(LGPS.check_online())
      {
        sprintf(buffer, "Actual position: http://www.google.com/maps/place/%f,%f?hl=es", LGPS.get_latitude(), LGPS.get_longitude());
        Serial.println(buffer);
/* - Hardcoded Begin - */  
  if(LSMS.ready())
  {
    LSMS.beginSMS(rnum);
    LSMS.print(buffer);
                    
    if(LSMS.endSMS()){
       Serial.println("SMS sent ok!");
       
       failOutSMS=false;
    }else{
      Serial.println("SMS send fail!");
       failOutSMS=true;
    }
  }else{
    Serial.println("SMS no ready!");
       failOutSMS=true;
  }
/* - Hardcoded End - */  

// - Harcode replacement 
//      failOutSMS = !sendSMS(buffer, rnum);

        if(!failOutSMS){
          newInSMS = false;           
        }
      }else{
        Serial.println("GPS Lost, retrying...");
        waitForGPS();
      }
    }
  }
  LSMS.flush();
  delay(1000);
}

boolean sendSMS(char* content, char* number)
{
  if(LSMS.ready())
  {
    LSMS.beginSMS(number);
    LSMS.print(content);
                    
    if(LSMS.endSMS()){
       Serial.println("SMS sent ok!");
       return true;
    }else{
      Serial.println("SMS send fail!");
      return false;
    }
  }else{
    Serial.println("SMS no ready!");
    return false;
  }
  

}

void waitForGPS(){
  
  while(!LGPS.check_online())
  {
      Serial.print("\r\nWaiting for GPS...");
      delay(1000);
  }
  Serial.println("FOUND!");
}


void checkBattery(){
    batteryLlevel = LBattery.level();
    Serial.print("Battery level is ");
    Serial.println(batteryLlevel);   
}
