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

#define _DEBUG_MODE true

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
  debug("From SMS to GPS Wearable POC.\r\n");
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
    debug(buffer);
          
    if((strcmp("GPS",(char*)buf_contex) == 0)||(newInSMS))
    {
      if(newInSMS){
        if(failOutSMS){
          debug("Retrying to send SMS.\r\n");
        }else{
          debug("Retrying after GPS lost.\r\n");
        }
      }else{
        debug("GPS in SMS content, going ahead!\r\n\r\n");
        newInSMS = true;
      }    

      if(LGPS.check_online())
      {
        sprintf(buffer, "Actual position: http://www.google.com/maps/place/%f,%f?hl=es\r\n", LGPS.get_latitude(), LGPS.get_longitude());
        debug(buffer);
/* - Hardcoded Begin - */  
  if(LSMS.ready())
  {
    LSMS.beginSMS(rnum);
    LSMS.print(buffer);
                    
    if(LSMS.endSMS()){
      debug("SMS sent ok!\r\n");
      failOutSMS=false;
    }else{
      debug("SMS send fail!\r\n");
      failOutSMS=true;
    }
  }else{
    debug("SMS no ready!\r\n");
    failOutSMS=true;
  }
/* - Hardcoded End - */  

// - Harcode replacement 
//      failOutSMS = !sendSMS((char*)buffer, (char*)rnum);

        if(!failOutSMS){
          newInSMS = false;           
        }
      }else{
        debug("GPS Lost, retrying...\r\n");
        waitForGPS();
      }
    }
  }
  LSMS.flush();
  delay(1000);
}

boolean sendSMS(char *content, char *number)
{
  if(LSMS.ready())
  {
    LSMS.beginSMS(number);
    LSMS.print(content);
                    
    if(LSMS.endSMS()){
       debug("SMS sent ok!\r\n");
       return true;
    }else{
      debug("SMS send fail!\r\n");
      return false;
    }
  }else{
    debug("SMS no ready!\r\n");
    return false;
  }
  

}

void waitForGPS(){
  
  while(!LGPS.check_online())
  {
      debug("\r\nWaiting for GPS...");
      delay(1000);
  }
  debug("FOUND!\r\n");
}


void checkBattery(){
    char ibuff[4];
    batteryLlevel = LBattery.level();
    itoa(batteryLlevel, ibuff, 10);
    debug("Battery level is ");
    debug(ibuff);
    debug("\r\n");
}

void debug(const char msg[])
{
  if(_DEBUG_MODE)Serial.print(msg);
}
