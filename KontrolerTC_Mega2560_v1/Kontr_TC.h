
#ifndef Kontr_TC_h
#define Kontr_TC_h

//#include "Configuration.h"

/*
#include <inttypes.h>

*/
#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h> 



extern void PreklopiCrpalkoTC(byte newState);
extern void Beep(unsigned char delayms);



//--------------------------------------------------------------------------------
void NastavitevPinov(void) {
  // init portov
  
  pinMode(53, OUTPUT);
  if (SD_CS_PIN != 53)
    pinMode(SD_CS_PIN, OUTPUT);
  
  if (ETHER_CS_PIN != 53)
    pinMode(ETHER_CS_PIN, OUTPUT);
  
  pinMode(BEEP_PIN, OUTPUT);
  
  pinMode(RELE_TC, OUTPUT);
  digitalWrite(RELE_TC, releState_TC);
  
  pinMode(RELE_CTC, OUTPUT);
  PreklopiCrpalkoTC(0);
  
  pinMode(RELE_CRAD, OUTPUT);
  digitalWrite(RELE_CRAD, R_CRAD_OFF);
  
  pinMode(VENTTC_1, OUTPUT);
  pinMode(VENTTC_2, OUTPUT);
  pinMode(VENTTC_EN, OUTPUT);
  digitalWrite(VENTTC_EN, HIGH);
  
  
  pinMode(CEVTERM_PEC_TC, INPUT_PULLUP);
  
  pinMode(STIKALO_CRP_RAD_ON, INPUT_PULLUP);
  pinMode(STIKALO_CRP_RAD_OFF, INPUT_PULLUP);
  pinMode(STIKALO_CRP_TC_ON, INPUT_PULLUP);
  pinMode(STIKALO_CRP_TC_OFF, INPUT_PULLUP);
  pinMode(STIKALO_TC_ON, INPUT_PULLUP);
  pinMode(STIKALO_TC_OFF, INPUT_PULLUP);
  
  
  
//  pinMode(CO_PWR_PIN, OUTPUT);
//  digitalWrite(CO_PWR_PIN, HIGH);
  
//  pinMode(CO_DOUT_PIN, INPUT);


}





//--------------------------------------------------------------------------------
void InitParametri(void) {
  char infoTime[9];
//  char infoSet[8];
//  unsigned int addrTmp;
 
/*   u2.uival = 0;
   for (int i=0; i<60; i++) {
     addrTmp = addrLastHourTemp + (unsigned int) i * sizeof(u2);
        
        delay(2);
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
 */ 
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4) );
  delay(2);
  onTimeTC = u4.ulval;
  Serial.print(F("OnTime: "));
  Serial.print(onTimeTC);
  Serial.println(F("s"));
  
  
  delay(5); 
  
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
  delay(2);
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
  
  
//  delay(5);
 // i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
  delay(2);
  prevTCState = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrPrevTCState, AT24C32_ADDR_LENGH);
  if (prevTCState != 0) {
    prevTCState = 0;
    lastTCStateChg = now();
    u4.ulval = lastTCStateChg;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
    delay(5);
    i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrPrevTCState, AT24C32_ADDR_LENGH, prevTCState);
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
  delay(2);
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(2);
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
  
 delay(2);
 i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(2);
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
  
  delay(2);
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(2);
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
          u2.uival = ((sumTemp[j] * 100) /(i)) + 5000;
       else
           u2.uival = 0;
        delay(2);

        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      sumTemp[j] += (float)((u2.uival/100.0)-50.0);
      delay(10);
    }
    Serial.print(j+1);
    Serial.print(F(": "));
    Serial.print(AvgVal(sumTemp[j], histLen*1.0), 3); 
    Serial.println(F(" ")); 
  }
  
  for (int j = numSens; j < numSens + numSensDHT22; j++) {
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
          u2.uival = ((sumTemp[j] * 100) /(i));
       else
           u2.uival = 0;
        delay(2);

        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      sumTemp[j] += (float)((u2.uival/100.0));
      delay(10);
    }
    Serial.print(j+1);
    Serial.print(F(": "));
    Serial.print(AvgVal(sumTemp[j], histLen*1.0), 3); 
    Serial.println(F(" ")); 
  }
  
}

#define STIKALO_ON   LOW
#define STIKALO_OFF  HIGH

#define STIKALO_STATE_AUT  0
#define STIKALO_STATE_ON   1
#define STIKALO_STATE_OFF  2

#define STIKALO_CRP_RAD 0
#define STIKALO_CRP_TC  1
#define STIKALO_TC      2

uint8_t stikaloState[3];
//uint8_t stikaloState02;
//uint8_t stikaloState03;

char stikaloStateTxt[3][4] = {"AUT", "ON", "OFF"};

//--------------------------------------------------------------------------------
void PreveriStikala(boolean izpisState) {
  
  if (digitalRead(STIKALO_CRP_RAD_ON) == STIKALO_ON)
    stikaloState[STIKALO_CRP_RAD] = STIKALO_STATE_ON;
  else if (digitalRead(STIKALO_CRP_RAD_OFF) == STIKALO_ON)
    stikaloState[STIKALO_CRP_RAD] = STIKALO_STATE_OFF;
  else
    stikaloState[STIKALO_CRP_RAD] = STIKALO_STATE_AUT;   
  
  if (digitalRead(STIKALO_CRP_TC_ON) == STIKALO_ON)
    stikaloState[STIKALO_CRP_TC] = STIKALO_STATE_ON;
  else if (digitalRead(STIKALO_CRP_TC_OFF) == STIKALO_ON)
    stikaloState[STIKALO_CRP_TC] = STIKALO_STATE_OFF;
  else
    stikaloState[STIKALO_CRP_TC] = STIKALO_STATE_AUT; 
  
  if (digitalRead(STIKALO_TC_ON) == STIKALO_ON)
    stikaloState[STIKALO_TC] = STIKALO_STATE_ON;
  else if (digitalRead(STIKALO_TC_OFF) == STIKALO_ON)
    stikaloState[STIKALO_TC] = STIKALO_STATE_OFF;
  else
    stikaloState[STIKALO_TC] = STIKALO_STATE_AUT;     
  
  if (izpisState) {
    for (int i=0; i<3; i++) {
      Serial.print(stikaloStateTxt[stikaloState[i]]);
      Serial.print(F(" "));
    }
  }  
  
/*  
  Serial.print(F("Stikalo crpalke rad: "));
  if (digitalRead(STIKALO_CRP_RAD_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_CRP_RAD_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));
  else
    Serial.println(F("AUTO")); 
    
  Serial.print(F("Stikalo crpalke TC: "));
  if (digitalRead(STIKALO_CRP_TC_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_CRP_TC_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));  
  else
    Serial.println(F("AUTO"));
  
  Serial.print(F("Stikalo 3: "));
  if (digitalRead(STIKALO_TC_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_TC_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));  
  else
    Serial.println(F("AUTO")); 
*/
}







#endif  
