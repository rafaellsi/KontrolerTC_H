#ifndef Kontr_TC_spl_h
#define Kontr_TC_spl_h

//#include "Temperature.h"



extern boolean IsNTempCas();
extern time_t processSyncMessage();
extern void EthernetInit(boolean izpisShort);
extern void EthernetIzpisInfo(void);
extern void SDInit(void);
extern void PrintTempAllSDbin(void);
extern void ImeDatotekeOnOff(char* ime);
extern void IzpisDataOnOffSerial(void);
extern void IzpisDatnaSerial(void);
extern unsigned int ObsegZgodovine(int sensor, unsigned int pred=0);
extern float AC_mimax(boolean izpis, boolean forceCalc);
extern  float Tok_12V(void);
extern void PreveriStikala(boolean izpisState);
extern void PrintTemperatureAll(void);
extern void PreveriNapetosti(boolean izpis, boolean internal, boolean external, boolean battery);


inline float Sec2Hour(unsigned long sec);
inline float AvgValFF_F(float suma, float num);
inline float AvgValULUL_F(unsigned long sum, unsigned long num);
float KompenzacijaTempOkolice(float tOkolice);
float KompenzZacTemp(float tStart);
float IzracDeltaTh();
float IzracDeltaThOk();
float IzracDeltaThSt();
float Cop(void);
boolean VodaVre(boolean izpis);
boolean MaxCrpTCRunTime();
float TempVklopaCrpTC_NTemp();
float TempIzklopaCrpTC_NTemp();
boolean RelaksTimeLimitSec(unsigned long cTime, unsigned long pTime,int rTime);
float TempVklopa(void);
float TempIzklopa(void);
float IzracunLimitTemp(int state, float ciTemp);
float IzracunTempVTOff(void);
boolean UpostevajElTarife(void);
void CheckSerial(void);

void IzpisPorabaWH(float porabaWH);
void IzracunHitrostiGretjaTC(void);
float PovpreciVred(float a, float povVred, float lastVred);
void ZapisiInIzpisiPodatke(void);
void PrintData(void);
float MejnaTempPreklCrpRad(byte newState);
int ZakasnitevVklopa(float temp, float mejnaTemp, int faktor);
float TempIzklopaCrpTC(void);
//unsigned int addrTmp;
//--------------------------------------------------------------------------------
// Pretvorba sekunde v ure, decimalno
inline float Sec2Hour(unsigned long sec)
{
  return((float)sec/(float)SECS_PER_HOUR);
}




//--------------------------------------------------------------------------------

inline float AvgValFF_F(float suma, float num)
{
  if (abs(num) < 0.0001)
    return(0.0);
  
  return(suma/num);
}

//--------------------------------------------------------------------------------
unsigned long cycStart;
unsigned long sumCycle;
unsigned long ncyc;
unsigned long minCycle = 0;
unsigned long maxCycle = 0;

//-----------------------
inline float AvgValULUL_F(unsigned long sum, unsigned long num)
{
  if (num > 0)
    return((float)sum/(float)num);
  return(-1.0);
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


boolean useDeltaThOk = false;
//--------------------------------------------------------------------------------------------
float IzracDeltaThOk() {
  float stevec;
  float imenovalec;
  float a;
  
  if (!useDeltaThOk) {
    return (0.0);
  }  
  
  a = deltaTh*Sec2Hour(lastRunTime)*KompenzZacTemp(startTemp);
  stevec = (cTemperatura[CRPALKA_0] - startTemp - a) * tKompOK;
  imenovalec = a * (tempOkolicaSt-tKompOK);
  
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
  
  stevec = (cTemperatura[CRPALKA_0] - startTemp - deltaTh * Sec2Hour(lastRunTime) * KompenzacijaTempOkolice(tempOkolicaSt)) * sqtKompStRef;

  imenovalec = deltaTh*Sec2Hour(lastRunTime)*(sqtKompStRef - sqtKompSt)*KompenzacijaTempOkolice(tempOkolicaSt);

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

//--------------------------------------------------------------------------------------------
float Cop(void)
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


//--------------------------------------------------------------------------------



float pTempCrp[3];
//--------------------------------------------------------------------------------
boolean VodaVre(boolean izpis) {
  
  float dTemp;
  
  if (cTemperatura[PEC_TC_DV] < tempVklopaCrpTC) {
    return(false); 
  } 
  
  dTemp = sq(pTempCrp[2] - cTemperatura[CRPALKA_0]);
  dTemp += (sq(pTempCrp[1] - cTemperatura[CRPALKA_0]));
  dTemp = sqrt(dTemp);
  
  if (izpis) {
    Serial.print(F(" <DTemp: "));
    Serial.print(dTemp);
    Serial.print(F(" "));
    Serial.print(pTempCrp[2]);
    Serial.print(F(" "));
    Serial.print(pTempCrp[1]);
    Serial.print(F("> "));
  }
  
  if (dTemp < 0.1)
    return(true);
  return(false);  
}  


//--------------------------------------------------------------------------------
boolean MaxCrpTCRunTime()
{
  return(RelaksTimeLimitSec(now(), lastCrpTCStateChg, 30*60)); // 30 min  
}

//--------------------------------------------------------------------------------
float TempVklopaCrpTC_NTemp() {
  float tmpTemp;
  
  if (!IsWeekend()) {
    tmpTemp = max(TempVklopa() + minDiffTCPec, (TempVklopa() +  ciljnaTemp)/2.0);
    return(tmpTemp);
  }
  return(ciljnaTemp - minDiffTCPec);
}

//--------------------------------------------------------------------------------
float TempIzklopaCrpTC_NTemp() {
  float tmpTemp;
  
  if (!IsWeekend()) {
    tmpTemp = max(TempIzklopa() + minDiffTCPec, histCrpTC + (TempVklopa() +  ciljnaTemp)/2.0);  
    return(tmpTemp);
  }
  return(ciljnaTemp + minDiffTCPec);
}

//-------------------------------------------------------------------------------- 
boolean RelaksTimeLimitSec(unsigned long cTime, unsigned long pTime,int rTime)
{
  if (cTime - pTime > (unsigned long)rTime)
    return(true);
  return(false);  
}



boolean izracHitrGret = false;
boolean izracHitrGretInfo=false;
boolean seRracunaHitrGret;

//--------------------------------------------------------------------------------
float TempVklopa(void)
{
  seRracunaHitrGret = false;
  
  if (IsWeekend()) {
    if (IsNTempCas()) {
        if (weekday() == NED && hour() > uraVTemp[0]) {
          seRracunaHitrGret = true;
          return(IzracunLimitTemp(0, ciljnaTemp));  
        }  
      return(minTempNightOn);    
    }
    else {
      return(105.0);
    }
  }
  else {
    if (!IsNTempCas()) {
      return(minTempVTOn);
    }
    if (!UpostevajElTarife()) {
        return(IzracunLimitTemp(0, minTempVTOn));  
      } 
    seRracunaHitrGret = true;
    return(IzracunLimitTemp(0, ciljnaTemp));
  }
}

//--------------------------------------------------------------------------------
float TempIzklopa(void)
{
  if (!IsWeekend()) {
    if (!IsNTempCas()) {
      return(IzracunTempVTOff());
    }
    if (!UpostevajElTarife())
        return(IzracunLimitTemp(1, minTempVTOn));    
    
    return(IzracunLimitTemp(1, ciljnaTemp));  
  }
  else {
    if (IsNTempCas()) {
      if (weekday() == NED && hour() > uraVTemp[0]) {
          return(IzracunLimitTemp(1, ciljnaTemp));  
      }  
      return(minTempNightOn + dTemp);
    }
    return(105.0); 
  }
}

//--------------------------------------------------------------------------------
//

float IzracunLimitTemp(int state, float ciTemp)
{
  float tUra;
  float refTemp;
  
  if (hour() < uraVTemp[0])
    tUra = (float)uraVTemp[0] - (float) hour()-1.0;  
  else 
    tUra = abs((float) hour()-24.0) + (float) uraVTemp[0] - 1.0;
  
  
//refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) / KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]);  
  
//  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0)));
//  refTemp *= KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]);
//  refTemp *= KompenzZacTemp(cTemperatura[CRPALKA_0]);
  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) * KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]) * KompenzZacTemp(cTemperatura[CRPALKA_0]);  


  
  if (state == 0) {  
//    return(max(minTempNightOn, refTemp + sensDiff/*+dTemp*/)); // !!!
    return(max(minTempNightOn, refTemp + deltaTh/2.0/*+dTemp*/)); // !!!
  }
  else if (state == 1) {
    if (hour() >= uraVTemp[0] - 2 && hour() < uraVTemp[1])
      return(ciTemp+(dTemp*2.0));
    else {  
//      refTemp = max(minTempNightOn+dTemp, refTemp+dTemp*2.0);
      refTemp = max(minTempNightOn+dTemp, refTemp+dTemp+deltaTh);
      return(min(ciTemp+(dTemp*2.0), refTemp));
    }
  }
  else 
    return(refTemp);
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
float IzracunTempVTOff(void)
{
  float rel;
  float relT;
  
  relT = (float) uraVTemp[1] - dolPrehObd/60.0;
  
  if ((float) hour() < relT)
    return(minTempVTOn + dTemp);
  
  rel = (float) hour() + (minute()/60.0) - relT;
  rel = rel/((float) uraVTemp[1] - relT);
  relT = minTempVTOn + dTemp*(1.0-rel);
  return(relT);
}


/*
static int FreeMemory() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
*/

//------------
boolean UpostevajElTarife(void)
{
  
  float diff_factor_01 = 4.0 * dTemp;
  float diff_factor_02 = 2.0 * dTemp;
  
  if (AvgValFF_F(sumTemp[OKOLICA_0], histLen*1.0) < (AvgValFF_F(sumTemp[PEC_TC_DV], histLen*1.0) - diff_factor_01) && 
      (AvgValFF_F(sumTemp[PEC_TC_DV], histLen*1.0) > (minTempVTOn + diff_factor_02 ))) {
    //minTempVTOn + dTemp
 //   minRunTimeMin = 30;
    return(false);
  }
 
//  minRunTimeMin = 45;
  return(true);
}


extern boolean debugDeviceHub;
//---------------------
void CheckSerial(void) {
//  char casun[5];
  char c;
  
  if (Serial.available() >  0 ) {
    c = Serial.peek();  
    
    if (Serial.available() >=  TIME_MSG_LEN ) {
//      casun[0]= c;  
      if (c /*casun[0]*/ ==  TIME_HEADER) {
        time_t t = processSyncMessage();
        if(t >0)  {
          RTC.set(t);   // set the RTC and the system time to the received value
          setTime(t);
          Serial.println(F("Cas je nastavljen"));
          prevCasMeritve = now();        
        }
      }   
    }    
    else if (Serial.available() == 3) {
     Serial.println(F(""));
     Serial.print(c);
       
     if (c == 'p') {
       c = Serial.read();
       c = Serial.read();
       Serial.print(c);
       if (c == 'o') {
         IzpisDataOnOffSerial();  
       }
       if (c == 'a') {
         IzpisDatnaSerial();
       }
       Serial.read();
     } // if p
     else if (c == 'm') {
       c = Serial.read();
       c = Serial.read();
       Serial.print(c);
       if (c == 'c') {
         c = Serial.read();
         Serial.print(c);
         if (c == '0') {
           manuCrpTCState = B01;  
         }
         else if (c == '1') {
           manuCrpTCState = B11;
         }
         else {
           manuCrpTCState = 0;
         }
         Serial.print(F("  "));
         Serial.print(manuCrpTCState);  
       }   
       else if (c == 'r') {      //mrX
         c = Serial.read();
         Serial.print(c);
         if (c == '0') {
           PreklopiCrpalkoRad(0);;  
         }
         else if (c == '1') {
           PreklopiCrpalkoRad(1);;
         }
       }       
     }  // if m**
     else if (c == 'e') {
       c = Serial.read();
       c = Serial.read();
       Serial.print(c);
       if (c == 'r') {
         c = Serial.read();
         Serial.print(c);
         if (c == 's') {
           EthernetInit(true);
         }  
       }
       else if (c == 'i') {
         c = Serial.read();
         Serial.print(c);
         if (c == 'i') {
           EthernetIzpisInfo();
         }  
       }
       else if (c == 'd') {
         c = Serial.read();
         Serial.print(c);
         if (c == '0') {
           debugDeviceHub = false;  
         }
         else {
           debugDeviceHub = true;  
         }    
       }
         
     }  // if e**
     else if (c == 's') {
       c = Serial.read();
       c = Serial.read();
       if (c == 'd') {
         c = Serial.read();
         Serial.print(c);
         if (c == 'r') {
           SDInit() ;
         }  
       }
     } // if s**
   } // if alaiable == 3
   if (Serial.available() >  0) {
     while (Serial.available() > 0) {
        Serial.print(Serial.read());
     }
     Serial.print(F(" "));    
   }  
//   Serial.flush(); 
  }  // if (Serial.available() >  0 )
//  Serial.flush(); 
}  

//---------------------
void IzpisPorabaWH(float porabaWH) {
  
  if (porabaWH >= 1000) {
    if (porabaWH/1000.0 > 100)
      Serial.print(porabaWH/1000.0, 1);
    else if (porabaWH/1000.0 > 10)
      Serial.print(porabaWH/1000.0, 2);
    else 
      Serial.print(porabaWH/1000.0, 3);  
    Serial.print(F("kVAh"));
  }
  else {
    if (porabaWH > 100)
      Serial.print(porabaWH, 1);
    else if (porabaWH > 10)
      Serial.print(porabaWH, 2);
    else
      Serial.print(porabaWH, 3);  
    Serial.print(F("VAh"));  
  }
}

//---------------------
void IzracunHitrostiGretjaTC(void) {
  
  float alpha;
  
      if (prevTCState == 0) {
        if (now() - lastTCStateChg > 15*60L) {  
          alpha = 0.99;
          if (izracHitrGret) {  
            alpha = 0.9;
          }
          else {
            alpha = 0.99;
          }
          
          lastDeltaTh = IzracDeltaTh();     
          deltaTh = PovpreciVred(alpha, deltaTh, lastDeltaTh);
          
          lastDeltaThOk = IzracDeltaThOk();
          deltaThOk = PovpreciVred(alpha, deltaThOk, lastDeltaThOk);
          
          lastDeltaThSt = IzracDeltaThSt();
          deltaThSt = PovpreciVred(alpha, deltaThSt, lastDeltaThSt);
                   
          Serial.println(F(""));
          if (izracHitrGretInfo) {
            Serial.println(F("Info - "));
          }
          
          Serial.print(F(" H.gret:"));
          Serial.print(deltaTh, 3);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaTh, 3);
          uf.fval =  deltaTh;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          Serial.print(F(" Komp.ok:"));
          Serial.print(deltaThOk, 4);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaThOk, 4);
          uf.fval =  deltaThOk;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          Serial.print(F(" Komp.st:"));
          Serial.print(deltaThSt, 4);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaThSt, 4);
          uf.fval =  deltaThSt;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          
          Serial.print(F(" Info cal.(startTemp): "));
          Serial.print(startTemp, 2);
          Serial.print(F(" temp okolice st: "));
          Serial.print(tempOkolicaSt, 2);
          Serial.print(F(" run time(h): "));
          Serial.print(Sec2Hour(lastRunTime), 4);
          Serial.print(F(" tkomp0: "));
          Serial.print(tKompOK, 2);
          Serial.print(F(" tkompSt: "));
          Serial.print(tKompSt, 2);
          
          Serial.print(F(" Nova komp.ok: "));
          Serial.print(KompenzacijaTempOkolice(tempOkolicaSt),4);
          Serial.print(F(" komp.st: "));
          Serial.print(KompenzZacTemp(startTemp),4);
          
          Serial.print(F(" Info deltaTh: "));
          Serial.print(IzracDeltaTh(),4);
          Serial.print(F(" Info deltaThOk: "));
          Serial.print(IzracDeltaThOk(),4);
          Serial.print(F(" Info deltaThSt: "));
          Serial.print(IzracDeltaThSt(),4);
          
          Serial.print(F(" Cop: "));
          Serial.print(Cop(),2);
          
          izracHitrGret=false;
          izracHitrGretInfo=false;
          if (izracHitrGret) {
            ZapisiOnOffSD(10, 100);
          }  
          else {
            ZapisiOnOffSD(11, 100);
          }        
        }
      }  

}
//--------------------------------------------------------------------------------------------

float PovpreciVred(float a, float povVred, float lastVred) {

  return((povVred * a) + ((lastVred)*(1.0-a)));
} 
//--------------------------------------------------------------------------------------------
static float PretvotiEETemp2Float(unsigned int u2uival) {
  
  
  return((u2uival/100.0)-50.0);
}

//--------------------------------------------------------------------------------------------
static unsigned int PretvoriFloat2EETemp(float temp) {
  
  return((temp+0.005+50.0)*100);
}


//--------------------------------------------------------------------------------------------
float sumPing = 0;
unsigned long numPing = 0;

int stateCevStPecTC;


float tok230V = -100.0;

extern unsigned long coRawValSum;
extern unsigned long numMerCO;
extern int coRawValMax;
extern int coRawValRef;

extern float minVoltGas;
extern float maxVoltGas;


//static uint8_t addr[4];
static word homePage(void);
//--------------------------------------------------------------------------------------------
void ZapisiInIzpisiPodatke(void) {
  unsigned int addrTmp; 
  
  Serial.println(F(""));   
    PrintData();
    
 //   printDataRF();
    
    
    Serial.print(F(" "));
    Serial.print(prevCrpTCState);
    Serial.print(prevVentTCState);
    Serial.print(prevCrpRadState);
    Serial.print(stateCevStPecTC);
    Serial.print(F(" "));
    Serial.print(manuCrpTCState, BIN);
    Serial.print(F(" "));
    Serial.print(preklopCrpTCVzr);

    
    if (UpostevajElTarife())
      Serial.print(F(" E"));
    else 
      Serial.print(F(" P"));  
    
    
    if (now()/((unsigned long)zapisXMin*60) > prevCasMeritve/((unsigned long)zapisXMin*60)) {     
      Serial.println(F(""));
//      for (int j = 0; j < numSensDS; j++) {
      for (int j = 0; j < numSens; j++) {
//        addrTmp = elapsedSecsToday(now())/(zapisXMin*60);
        addrTmp = ObsegZgodovine(j);
//        addrTmp += (j * histLen);    
//        addrTmp *= sizeof(u2);
//        addrTmp += addrTempBack;
 //     addrTmp = addrTempBack + sizeof(uf)*((elapsedSecsToday(now())/(zapisXMin*60)) + (day()-1)*144); //1440/zapisXMin
        delay(2);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        delay(2);

        Serial.print(F(" Ta"));
        Serial.print(j+1);
        Serial.print(F(":"));
        
        Serial.print(F("("));
//        Serial.print((u2.uival/100.0)-50.0);
        Serial.print(PretvotiEETemp2Float(u2.uival));
        
        Serial.print(F(">"));
        Serial.print(addrTmp);
        
        Serial.print(F(")"));
        
        // dodano kot kontrola vrenja
        if (j == CRPALKA_0) {
          pTempCrp[2] = pTempCrp[1];
          pTempCrp[1] = pTempCrp[0];
          pTempCrp[0] = cTemperatura[j];
          if (VodaVre(false))
            Serial.print(F("P "));
          else
            Serial.print(F("V "));  
        }
        // dodano kot kontrola vrenja  
        
 //       float diff = cTemperatura[j] - ((u2.uival/100.0)-50.0);
        float diff = cTemperatura[j] - PretvotiEETemp2Float(u2.uival); 
        
        if (abs(diff) > 0.01) {
          //sumTemp[j] -= (((u2.uival)/100.0) - 50.0);
          sumTemp[j] -= PretvotiEETemp2Float(u2.uival);
//          u2.uival =  (cTemperatura[j]+0.005+50.0)*100;
          u2.uival =  PretvoriFloat2EETemp(cTemperatura[j]);

          sumTemp[j] += (cTemperatura[j]);

          delay(2);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
          delay(2);
        }

        Serial.print(AvgValFF_F(sumTemp[j], histLen*1.0),2);

      }
      
      for (int j = numSens; j < numSens + numSensDHT22; j++) {
        Serial.println(F(""));
        addrTmp = ObsegZgodovine(j);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        Serial.print(F(" RHa"));
        Serial.print(j-numSens+1);
        Serial.print(F(":"));
        
        Serial.print(F("("));
        Serial.print(u2.uival/100.0);
        
        Serial.print(F(">"));
        Serial.print(addrTmp);
        
        Serial.print(F(")"));
        float diff = cVlaznost[j-numSens] - (u2.uival/100.0);
        if (abs(diff) > 0.01) {
          sumTemp[j] -= (u2.uival)/100.0;
          u2.uival =  (cVlaznost[j-numSens]+0.005)*100;
          sumTemp[j] += (cVlaznost[j-numSens]);
        
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
          delay(2);
        }
        Serial.print(AvgValFF_F(sumTemp[j], histLen*1.0),2);
      }
      Serial.println(F(""));

          
      Serial.print(F(" Avg.cycleTime(n="));
      Serial.print(ncyc);
      Serial.print(F("): "));
      Serial.print(AvgValULUL_F(sumCycle, ncyc));
      Serial.print(F("ms "));    
      Serial.print(F("Min-Max(last h): "));
      Serial.print(minCycle);
      Serial.print(F("-"));
      Serial.print(maxCycle);
      maxCycle = 0;
      
      Serial.print(F(" Avg.ping time(n="));
      Serial.print(numPing);
      Serial.print(F("): "));
      Serial.print(AvgValULUL_F(sumPing, numPing));
      Serial.print(F("ms ")); 
      

      
//      PreveriNapetosti(true, true, false);
      Serial.println(F(""));
    }
    PrintTempAllSDbin();  
    
//    Serial.println("");
//    Serial.print(F("       "));
    
    Serial.print(F(" Tok: "));
    tok230V = AC_mimax(showCRC, true);
    Serial.print(tok230V);  
    Serial.print(F("A("));
    
//    porabaWH += (tok *(230.0/60.0));
    IzpisPorabaWH(porabaWH);
    
    Serial.print(F(") "));
    
    Serial.flush();
    Serial.print(F("On time: "));
    
     

    if (prevTCState == 0) {
      if (onTimeTC > 0) 
      Serial.print(Sec2Hour(onTimeTC));  
    }
    else if (prevTCState == 1)
       Serial.print(Sec2Hour(onTimeTC + now() - lastTCStateChg));
    
    Serial.print("/");
    if (prevCrpTCState == 0) {
      Serial.print(Sec2Hour(onTimeCrpTC)); 
    }
    else {
      Serial.print(Sec2Hour(onTimeCrpTC + now() - lastCrpTCStateChg));  
    }  
    
    Serial.print("/");
    
    if (prevCrpRadState == 0) {
      Serial.print(Sec2Hour(onTimeCrpRad)); 
    }
    else {
      Serial.print(Sec2Hour(onTimeCrpRad + now() - lastCrpRadStateChg));  
    }  
    
    
    Serial.println("");

     
     Serial.print(F(" Tv:"));
     Serial.print(TempVklopa(), 3);  
     Serial.print(F(" Ti "));
     Serial.print(TempIzklopa(), 1);
     Serial.print(F(" ("));
     Serial.print(TempVklopaCrpTC_NTemp(), 1);
     Serial.print(F("-"));
     Serial.print(TempIzklopaCrpTC_NTemp(), 1);
     Serial.print(F("->"));
     Serial.print(MejnaTempPreklCrpRad(0), 1);
     Serial.print(F("-"));
     Serial.print(MejnaTempPreklCrpRad(1), 1);
//     Serial.print(F(") "));
     
     Serial.print(F(") CO:"));
     Serial.print(coRawValRef);
     Serial.print(F("("));
     Serial.print(AvgValULUL_F(coRawValSum, numMerCO), 3);
     Serial.print(F("/"));
     Serial.print(coRawValMax);
     Serial.print(F(")"));
     Serial.print(F(" "));
     
     Serial.print(osvetlitevLCD);
     
     Serial.print(F(" I(12v):"));
     Serial.print(Tok_12V());
     Serial.print(F("("));
     Serial.print(AvgValFF_F(sumTok_12V, (float) nMerTok_12V), 4);
//     Serial.print(sumTok_12V/((float) nMerTok_12V), 3);
     Serial.print(F("/"));
     Serial.print(maxTok_12V);
     Serial.print(F(")A "));  
     
     PreveriNapetosti(true, true, true, false);
 /*    
     Serial.print(F(" ("));
     Serial.print(minVoltGas);
     Serial.print(F("/"));
     Serial.print(maxVoltGas);
     Serial.print(F(")V ")); 
 */    
     PreveriStikala(true);
     
      Serial.print(F(" dcnt:"));
      Serial.print((unsigned int)ether.delaycnt);
      Serial.print(F(" LU:"));
      if (ether.isLinkUp() == true)
        Serial.print(F("t"));
      else
        Serial.print(F("f"));  
  
  
//  ether.printIp(" addr: ", addr);
//  ether.sendUdp((char*)homePage(), sizeof(ether.buffer[500]), 80, addr, 10002);
	


}


//--------------------------------------------------------------------------------
// main function to print information about a device
void PrintData(void)
{
  char cas[13];
 
  NarediTimeStr(cas, now());
  Serial.print(cas);

  Serial.print(F(" -> "));
  PrintTemperatureAll();
  
  if (releState_TC == R_TC_ON) {
    if (prevTCState == 1) 
       Serial.print(F("ON"));
     else
       Serial.print(F("SB"));
   }
   else
     Serial.print(F("OFF"));

}

//--------------------------------------------------------------------------------
float MejnaTempPreklCrpRad(byte newState)
{
  // dodaj kot glob
  int ref_prostor_1 = 0;
  float kTemp = 0.25;
  float cTemperaturaZun = 0.0;
  // mogce spremeni abs temp
  float zeljTemp;
  
  
  float refTemp;
  float tmpTemp;
  float dTemp = 0.0;
  
  refTemp = (float)limTempCrpRad[hour()] + limTempFactCrpRad[hour()] * cTemperatura[RAD_DV];
  if (newState == 1) {
    tmpTemp = cTemperatura[RAD_DV] - refTemp;
    if (isCrpRadAsAbsTemp) {
      zeljTemp =  (float)crpRadAsAbsTemp[hour()] - 10.0;
 
        dTemp = (zeljTemp - cTemperaturaZun) * kTemp; 
 //       if (cTemperatura[ref_prostor_1] < zeljTemp) {
        dTemp *= (zeljTemp / cTemperatura[ref_prostor_1]);
 //     }
      tmpTemp = min((float)crpRadAsAbsTemp[hour()] + dTemp, tmpTemp);
    }
    if (cTemperatura[PEC_DV] >  maxTempDVPec) {
      return(maxTempDVPec);  
    }
    if (tmpTemp < 5.00)  {
      return(5.0);
    }  
    return(tmpTemp);
  }
  else {
    refTemp *= minMejnaTempRel;
    if (refTemp < 5.0)
      refTemp = 5.0;
    if (cTemperatura[RAD_DV] - refTemp > maxTempPVRad)
      return(maxTempPVRad);
    
    if (cTemperatura[RAD_DV] - refTemp < 5.00)  {
      return(5.0);
    }  
    return(cTemperatura[RAD_DV] - refTemp);
  }
}


//--------------------------------------------------------------------------------
float TempIzklopaCrpTC(void) 
{
  return(tempVklopaCrpTC - histCrpTC);
}  

//--------------------------------------------------------------------------------  
int ZakasnitevVklopa(float temp, float mejnaTemp, int faktor) {
  if (temp > mejnaTemp) {
    return(faktor * zaksnitevCrpVent_Sec);
  }
  return (zaksnitevCrpVent_Sec);  
}  

/*
#ifndef ARDPRINTF
#define ARDPRINTF
#define ARDBUFFER 16
#include <stdarg.h>
#include <Arduino.h>

int ardprintf(char *str, ...)
{
  int i, count=0, j=0, flag=0;
  char temp[ARDBUFFER+1];
  for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++;

  va_list argv;
  va_start(argv, count);
  for(i=0,j=0; str[i]!='\0';i++)
  {
    if(str[i]=='%')
    {
      temp[j] = '\0';
      Serial.print(temp);
      j=0;
      temp[0] = '\0';

      switch(str[++i])
      {
        case 'd': Serial.print(va_arg(argv, int));
                  break;
        case 'l': Serial.print(va_arg(argv, long));
                  break;
        case 'f': Serial.print(va_arg(argv, double));
                  break;
        case 'c': Serial.print((char)va_arg(argv, int));
                  break;
        case 's': Serial.print(va_arg(argv, char *));
                  break;
        default:  ;
      };
    }
    else 
    {
      temp[j] = str[i];
      j = (j+1)%ARDBUFFER;
      if(j==0) 
      {
        temp[ARDBUFFER] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };
  Serial.println();
  return count + 1;
}
#undef ARDBUFFER
#endif
*/


#endif
