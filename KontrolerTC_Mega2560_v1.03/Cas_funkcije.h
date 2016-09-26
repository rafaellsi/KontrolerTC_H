#ifndef Cas_funkcije_h
#define Cas_funkcije_h


void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec);
boolean IsWeekend(void);
boolean  PreveriCas(void);
unsigned long getNtpTime(void);
uint8_t DstSet(void);

//--------------------------------------------------------------------------------
// Naredi string v obliki HH:MM:SS
//  - izpSec:    - true, izpiše tudi secunde
//               - false, brez sekund   
void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec = true)
{   
  if (izpSec)
    sprintf(cas, "%02d:%02d:%02d", hour(tcas), minute(tcas), second(tcas));
  else 
    sprintf(cas, "%02d:%02d", hour(tcas), minute(tcas));
}

//--------------------------------------------------------------------------------
//
boolean IsWeekend(void)
{
  if (weekday() == NED || weekday() == SOB)
    return(true);
  return(false);  
}

//--------------------------------------------------------------------------------
//
boolean PreveriCas(void)
{
  int n = 0;
  boolean timeIsOk = false;
  unsigned long ntpTime = 0;


 DstSet();    
 while (ntpTime == 0 && n < 3) {
 
    ntpTime = getNtpTime();
    n++;
    if (ntpTime > 0) {
      Serial.print(" Diff.t:");
      Serial.print(ntpTime-now());
      if (abs(now() - ntpTime) > 5) {
        RTC.set(ntpTime);   // set the RTC and the system time to the received value
        setTime(ntpTime);
        Serial.print("->");
        Serial.print(now());
        Serial.print("<");
      }
      Serial.print(" OK");
      timeIsOk = true;
      break;
    }
  }
  return(timeIsOk);
}


//--------------------------------------------------------------------------------
//
uint8_t DstSet(void) {

  boolean dst = true;
  byte tmpDST;

  prevDayLightSaving = dayLightSaving;
  
  if (dayLightSaving == 0) {
    dst = false;
  } else {
    dayLightSaving = 0;   
  }
  
  if (month() > 3 && month() < 10) {
    dayLightSaving = 1;
    if (prevDayLightSaving != dayLightSaving) {
      tmpDST = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH);
      if (tmpDST != dayLightSaving) {
        i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH, dayLightSaving);
      }
    }  
  }
  else {
    if (month() == 3) {
      if (day() + (7 - weekday()) >= 31) {
        if (hour() >= 2 /*&& !dst*/) {
          dayLightSaving = 1;
          if (prevDayLightSaving != dayLightSaving) {
            tmpDST = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH);
            if (tmpDST != dayLightSaving) {
              i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH, dayLightSaving);
            }
          }      
              //zapiši v Epprom
            //nastavi RTC
            //nastavi v time   
        }
      }
    }
    else if (month() == 10) {
      if (day() + (7 - weekday()) < 31) {
        if (hour() < 3 /*&& !dst*/) {
          dayLightSaving = 1;
          if (prevDayLightSaving != dayLightSaving) {
            tmpDST = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH);
            if (tmpDST != dayLightSaving) {
              i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH, dayLightSaving);
            }
          }  
        }
      }
    }
  }
  if (dst && dayLightSaving == 0) {
    tmpDST = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH);
    if (tmpDST != dayLightSaving) {
      i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrDayLightSaving, AT24C32_ADDR_LENGH, dayLightSaving);
    }  
    //nastavi RTC
    //nastavi v time 
  }
  if (prevDayLightSaving != dayLightSaving) {
    Serial.println("");
    Serial.println("New DST:");
    Serial.print(dayLightSaving);
    Serial.print(" Old DST:");
    Serial.println(prevDayLightSaving);
  }

  
  return(dayLightSaving);
}




#endif
