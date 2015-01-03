
#ifndef Kontroler_TC_h
#define Kontroler_TC_h

//#include "Configuration.h"

/*
#include <inttypes.h>

*/
#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h> 



extern void PreklopiCrpalkoTC(byte newState);

float KompenzacijaTempOkolice(float tOkolice);
float KompenzZacTemp(float tStart);
static float AvgVal(float suma, float num);

//--------------------------------------------------------------------------------
void NastavitevPinov(void) {
  // init portov
  pinMode(BEEP_PIN, OUTPUT);
  
  pinMode(RELE_TC, OUTPUT);
  digitalWrite(RELE_TC, releState_1);
  
  pinMode(RELE_CTC, OUTPUT);
  PreklopiCrpalkoTC(0);
  
  pinMode(RELE_CRAD, OUTPUT);
  digitalWrite(RELE_CRAD, R_CRAD_OFF);
  
  pinMode(VENTTC_1, OUTPUT);
  pinMode(VENTTC_2, OUTPUT);
  pinMode(VENTTC_EN, OUTPUT);
  digitalWrite(VENTTC_EN, HIGH);
  
  pinMode(SD_CS_PIN, OUTPUT);
  
  pinMode(CEVTERM_PEC_TC, INPUT_PULLUP);
  
//  pinMode(CO_PWR_PIN, OUTPUT);
//  digitalWrite(CO_PWR_PIN, HIGH);
  
//  pinMode(CO_DOUT_PIN, INPUT);


}

//--------------------------------------------------------------------------------
// Naredi string v obliki HH:MM:SS
//  - izpSec:    - true, izpiše tudi secunde
//               - false, brez sekund   
static void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec = true)
{   
  if (izpSec)
    sprintf(cas, "%02d:%02d:%02d", hour(tcas), minute(tcas), second(tcas));
  else 
    sprintf(cas, "%02d:%02d", hour(tcas), minute(tcas));
}


//--------------------------------------------------------------------------------
// Pretvorba sekunde v ure, decimalno
static float Sec2Hour(unsigned long sec)
{
  return((float)sec/(float)SECS_PER_HOUR);
}

  unsigned int addrTmp;
//--------------------------------------------------------------------------------
void InitParametri(void) {
  char infoTime[9];
//  char infoSet[8];
//  unsigned int addrTmp;
  
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4) );
  delay(5);
  onTimeTC = u4.ulval;
  Serial.print(F("OnTime: "));
  Serial.print(onTimeTC);
  Serial.println(F("s"));

  delay(5); 
  
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
  delay(5);
  lastTCStateChg = u4.ulval;
  Serial.print(F("Last TC chg: "));
  NarediTimeStr(infoTime, lastTCStateChg);
  Serial.print(day(lastTCStateChg));
  Serial.print(F("."));
  Serial.print(month(lastTCStateChg));
  Serial.print(F("."));
  Serial.print(year(lastTCStateChg)); 
  Serial.print(F("  "));
  Serial.print(infoTime);
  
  delay(5);
 // i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
  prevTCState = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH);
  if (prevTCState != 0) {
    prevTCState = 0;
    lastTCStateChg = now();
    u4.ulval = lastTCStateChg;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
    delay(5);
    i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
    Serial.print(F(  "\"prevTCState\" nastavljeno na "));
    Serial.println(prevTCState);
  } 
  else {
    Serial.println("");
  }  
/*
  Serial.print("sizeof(ux):");
  Serial.println(sizeof(u2));
  Serial.println(sizeof(u4));
  Serial.println(sizeof(uf));
*/  
  delay(5);
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaTh = uf.fval;
  Serial.print(F("Hir. gret: "));
  Serial.print(deltaTh, 3);
  if (deltaTh <= 0 || deltaTh > 25.0) {
    deltaTh = 4.35;
    Serial.print(F(" / "));
    Serial.print(deltaTh, 3);
    uf.fval =  deltaTh;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));

  }
  Serial.println(F("K/h"));
  
 delay(5);
 i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaThOk = uf.fval;
  Serial.print(F("Komp.ok.: "));
  Serial.print(deltaThOk, 4);
  if (abs(deltaThOk) > 100.0) {
    deltaThOk = 0.0;
    Serial.print(F(" / "));
    Serial.print(deltaThOk, 4);
    uf.fval =  deltaThOk;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  }
  Serial.println(F("")); 

  Serial.print(F("Kompenz. temp. okolice pri temp. "));
  Serial.print(cTemperatura[OKOLICA_0]);
  Serial.print(F("C je "));
  Serial.println(KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]),4);
  
  delay(5);
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaThSt = uf.fval;
  Serial.print(F("Komp.st.: "));
  Serial.print(deltaThSt, 4);
  if (abs(deltaThSt) > 100.0) {
    deltaThSt = 0.0;
    Serial.print(F(" / "));
    Serial.print(deltaThSt, 4);
    uf.fval =  deltaThSt;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  }
  Serial.println(F("")); 
  
  Serial.print(F("Kompenz. zacetne temp. pri temp. "));
  Serial.print(cTemperatura[CRPALKA_0]);
  Serial.print(F("C je "));
  Serial.println(KompenzZacTemp(cTemperatura[CRPALKA_0]),4);

  
  for (int j = 0; j < numSens; j++) {
    Serial.print(F("Addres:"));
    Serial.print(addrTempBack + ((j*histLen))*sizeof(u2));
    Serial.print(F("  "));
    for (unsigned int i=0; i < histLen; i++) {   // 7*1440/marXMin
      addrTmp =  addrTempBack + (i + (j*histLen))*sizeof(u2);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);
      
//      if ((u2.uival) < 1 || (u2.uival) > 12000) {
      if (isnan(u2.uival)) {
        if (i > 0)
          u2.uival = (sumTemp[j] * 100) /(i);
        else
          u2.uival = 0;
        delay(2);

        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      sumTemp[j] += (float)(u2.uival/100.0);
      delay(10);
    }
    Serial.print(j+1);
    Serial.print(F(": "));
    Serial.print(AvgVal(sumTemp[j], histLen*1.0), 3); 
    Serial.println(F(" "));  
  }    
}

//--------------------------------------------------------------------------------

static float AvgVal(float suma, float num)
{
  return(suma/num);
}
//--------------------------------------------------------------------------------
//
static float RefTemp()
{
 // return(AvgAllTimeTemp(CRPALKA_0));
  return(cTemperatura[CRPALKA_0]);
}


//--------------------------------------------------------------------------------------------
float KompenzacijaTempOkolice(float tOkolice)
{
//  return(1 + (20.0 - tOkolice) * deltaThOk);
  float komp;
  
  if (tKompOK == 0) {
    komp = 1.0;
  }
  else {
    komp = (((tOkolice-tKompOK)/tKompOK)*deltaThOk);
    komp = 1.0 + komp;
//    komp = (1.0 - (deltaThOk*((tOkolice-tKomp0)/tKomp0)));  
  }  
  
   return(komp);
}

//--------------------------------------------------------------------------------------------
float KompenzZacTemp(float tStart)
{
  float komp;
  float sqtStart;
  float sqtKompST;
 
 /* 
  if (tStart <= tKompSt) {
//    komp = 1.0;
    return(1.0);    
  }  
*/
  if (abs(tKompSt) < 0.1) {
//    komp = 1.0;
    return(1.0);
  }  
  else {
    sqtStart = tStart*tStart;
    sqtKompST = tKompSt*tKompSt;
 //   komp = tKompSt*tKompSt - sqtStart;
 //   komp = komp / (sqtStart);
 //   komp = komp * deltaThSt;
   
  ////  komp = (((tKompSt*tKompSt - tStart*tStart)/tStart*tStart)*deltaThSt);
  //  komp = (1 + komp);
    
     komp = 1+ (((sqtKompST - sqtStart)/sqtKompST)*deltaThSt);
  }
  return(komp); 
}


//--------------------------------------------------------------------------------------------

float IzracDeltaTh() {

  float imenovalec;
  
  imenovalec = (KompenzacijaTempOkolice(tempOkolicaSt) * KompenzZacTemp(startTemp) * (Sec2Hour(lastRunTime)));
  
  if (abs(imenovalec) < 0.001)
    return(deltaTh);
  return((cTemperatura[CRPALKA_0]- startTemp) / imenovalec);
  
}  


//--------------------------------------------------------------------------------------------
float IzracDeltaThOk() {
  float stevec;
  float imenovalec;
  float a;
  
//  stevec = (startTemp - cTemperatura[CRPALKA_0] + deltaTh* Sec2Hour((float)lastRunTime) * KompenzZacTemp(startTemp));
//  stevec = stevec * tKomp0;
  
  a = deltaTh*Sec2Hour(lastRunTime)*KompenzZacTemp(startTemp);
  stevec = (cTemperatura[CRPALKA_0] - startTemp - a) * tKompOK;
  imenovalec = a * (tempOkolicaSt-tKompOK);
  
//  stevec = (startTemp - cTemperatura[CRPALKA_0] + deltaTh*Sec2Hour(lastRunTime)*KompenzZacTemp(startTemp)) * tKompOK;

//  imenovalec = (tempOkolicaSt-tKomp0);
//  imenovalec *= deltaTh;
//  imenovalec *= Sec2Hour((float)lastRunTime);
//  imenovalec *= KompenzZacTemp(startTemp);

//  imenovalec = deltaTh*Sec2Hour(lastRunTime)*(tempOkolicaSt-tKompOK)*KompenzZacTemp(startTemp);
  
//  if (abs(imenovalec) > 0.01)
//  if (imenovalec < 0.01 && imenovalec > -0.01)
  if (abs(imenovalec) < 0.0001)
    return(deltaThOk);
  
  stevec = stevec/imenovalec;
  
  if (stevec > 0) {
    if (deltaThOk > 0.5 && stevec > maxDeltaDev * deltaThOk) {
      return(maxDeltaDev * deltaThOk);  
    } 
    return(stevec);
  }
  return(0.0);  
}   




//--------------------------------------------------------------------------------------------
float IzracDeltaThSt() {
  float stevec;
  float imenovalec;
  
  float sqtKompSt = startTemp*startTemp;
  float sqtKompStRef = tKompSt*tKompSt;
   
 // stevec = cTemperatura[CRPALKA_0] - startTemp - (deltaTh * Sec2Hour((float)lastRunTime) * KompenzacijaTempOkolice(tempOkolicaSt)); 
 // stevec = stevec * sqtKompSt; 
  
  stevec = (cTemperatura[CRPALKA_0] - startTemp - deltaTh * Sec2Hour(lastRunTime) * KompenzacijaTempOkolice(tempOkolicaSt)) * sqtKompStRef;
  
 // imenovalec = (tKompSt*tKompSt - sqtKompSt);
 // imenovalec *= deltaTh;
 // imenovalec *= Sec2Hour((float)lastRunTime);
 // imenovalec *= KompenzacijaTempOkolice(tempOkolicaSt);

  imenovalec = deltaTh*Sec2Hour(lastRunTime)*(sqtKompStRef - sqtKompSt)*KompenzacijaTempOkolice(tempOkolicaSt);
  
 // if (abs(imenovalec) > 0.01)
 //   return(stevec/imenovalec);
 // return(deltaThSt);  
  if (abs(imenovalec) < 0.0001)
    return(deltaThSt);
  
  stevec = stevec/imenovalec;
  if (stevec > 0) {
    if (deltaThSt > 0.5 && stevec > maxDeltaDev * deltaThSt) {
      return(maxDeltaDev * deltaThSt);  
    }  
    return(stevec);
  }
  return(0.0);    
} 


static float Cop(void)
{
  //float Q;
  //float W;
  /*
  c = 4200; //J/kgK
  m = 230; //kapaciteta bojlerja 
  
  dT = cTemperatura[CRPALKA_0] - startTemp;
  Q = c * m * dT;
  W = (porabaWh - zacPorabaWh) * 3600; //VAh *3600
  */
  Qv = 4200.0 * 230.0 * (cTemperatura[CRPALKA_0] - startTemp);
  We = (porabaWH - zacPorabaWH) * 3600.0;
  if (We > 0)
    return(Qv/We);
  return(0);  
}


void Beep(unsigned char delayms) {
  digitalWrite(BEEP_PIN, HIGH);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  digitalWrite(BEEP_PIN, LOW);       // 0 turns it off  
}  


#define CO_HEATING_STATE_OFF 0
#define CO_HEATING_STATE_HIGH  1
#define CO_HEATING_STATE_LOW  2
#define CO_HEATING_OFF  HIGH
#define CO_HEATING_ON  LOW
#define CO_HEATING_PWM_LOW 184

//#define CO_INIT

#ifdef CO_INIT
  int numMerCO=0;
#endif

int coRawVal;


void PreveriCO_Senzor() {
  static unsigned long t_CO_timer = 0;
  static unsigned long t_CO_nextHChange = 0;
  static byte co_sens_heat_level = CO_HEATING_STATE_OFF;
  pinMode(CO_PWR_PIN, OUTPUT);
  pinMode(CO_DOUT_PIN, INPUT);
  
  if (now() > t_CO_nextHChange) {
    if (co_sens_heat_level == CO_HEATING_STATE_LOW || co_sens_heat_level == CO_HEATING_STATE_OFF) {


#ifdef CO_INIT
      t_CO_nextHChange = now() + 175000;
#else      
      t_CO_nextHChange = now() + 90;
      coRawVal = analogRead(CO_SENS_APIN);
#endif
      digitalWrite(CO_PWR_PIN, LOW);
/*
      Serial.print(" ");
      Serial.print(now());
      Serial.print(" ");
      Serial.print(t_CO_nextHChange);
*/      
      
      co_sens_heat_level = CO_HEATING_STATE_HIGH;
      Serial.print(F(">"));
//      Serial.print(F("CO High"));
    }  
    else if (co_sens_heat_level == CO_HEATING_STATE_HIGH) {
      analogWrite(CO_PWR_PIN, CO_HEATING_PWM_LOW);
      t_CO_nextHChange = now() + 60;
      co_sens_heat_level = CO_HEATING_STATE_LOW;
      Serial.print(F("<"));
//      Serial.print(F("CO Low"));
    }   
  }
  if (digitalRead(CO_DOUT_PIN) == HIGH) {
    Serial.print(F("CO alarm"));
    Beep(50);
  }  
#ifdef CO_INIT  
  coRawVal += analogRead(CO_SENS_APIN);
  numMerCO++;
#endif
}  

#endif 
