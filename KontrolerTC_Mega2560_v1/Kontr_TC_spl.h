#ifndef Kontr_TC_spl_h
#define Kontr_TC_spl_h

//#include "Temperature.h"

extern boolean IsNTempCas();
extern time_t processSyncMessage();
extern void EthernetInit(boolean izpisShort);
extern void SDInit(void);
extern void ImeDatotekeOnOff(char* ime);

static float Sec2Hour(unsigned long sec);
static float AvgVal(float suma, float num);
float KompenzacijaTempOkolice(float tOkolice);
float KompenzZacTemp(float tStart);
float IzracDeltaTh();
float IzracDeltaThOk();
float IzracDeltaThSt();
static float Cop(void);
static float AvgCycleTime(unsigned long sum, unsigned long num);
static boolean VodaVre(boolean izpis);
static boolean MaxCrpTCRunTime();
float TempVklopaCrpTC_NTemp();
static float TempIzklopaCrpTC_NTemp();
static boolean RelaksTimeLimitSec(unsigned long cTime, unsigned long pTime,int rTime);
float TempVklopa(void);
float TempIzklopa(void);
static float IzracunLimitTemp(int state, float ciTemp);
static float IzracunTempVTOff(void);
static boolean UpostevajElTarife(void);
void CheckSerial(void);
static void IzpisDataOnOffSerial(void);
static void IzpisDatnaSerial(void);


//--------------------------------------------------------------------------------
// Pretvorba sekunde v ure, decimalno
static float Sec2Hour(unsigned long sec)
{
  return((float)sec/(float)SECS_PER_HOUR);
}

  unsigned int addrTmp;


//--------------------------------------------------------------------------------

static float AvgVal(float suma, float num)
{
  if (num != 0)
    return(suma/num);
  return(0.0);
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


//--------------------------------------------------------------------------------
unsigned long cycStart;
unsigned long sumCycle;
unsigned long ncyc;
unsigned long minCycle = 0;
unsigned long maxCycle = 0;

//--------------------------------------------------------------------------------
static float AvgCycleTime(unsigned long sum, unsigned long num)
{
  if (num > 0)
    return((float)sum/(float)num);
  return(-1.0);
}


float pTempCrp[3];
//--------------------------------------------------------------------------------
static boolean VodaVre(boolean izpis) {
  float dTemp;
  
//  dTemp = (float) abs((float) pTempCrp[2] - (float) cTemperatura[CRPALKA_0]);
//  dTemp += ((float) abs((float) pTempCrp[1] - (float) cTemperatura[CRPALKA_0]));
  
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

unsigned long lastCrpTCStateChg;
//--------------------------------------------------------------------------------
static boolean MaxCrpTCRunTime()
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
static float TempIzklopaCrpTC_NTemp() {
  float tmpTemp;
  
  if (!IsWeekend()) {
    tmpTemp = max(TempIzklopa() + minDiffTCPec, histCrpTC + (TempVklopa() +  ciljnaTemp)/2.0);  
    return(tmpTemp);
  }
  return(ciljnaTemp + minDiffTCPec);
}

//-------------------------------------------------------------------------------- 
static boolean RelaksTimeLimitSec(unsigned long cTime, unsigned long pTime,int rTime)
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

static float IzracunLimitTemp(int state, float ciTemp)
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
static float IzracunTempVTOff(void)
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
static boolean UpostevajElTarife(void)
{
  
  float diff_factor_01 = 4.0 * dTemp;
  float diff_factor_02 = 2.0 * dTemp;
  
  if (AvgVal(sumTemp[OKOLICA_0], histLen*1.0) < (AvgVal(sumTemp[PEC_TC_DV], histLen*1.0) - diff_factor_01) && 
      (AvgVal(sumTemp[PEC_TC_DV], histLen*1.0) > (minTempVTOn + diff_factor_02 ))) {
    //minTempVTOn + dTemp
 //   minRunTimeMin = 30;
    return(false);
  }
 
//  minRunTimeMin = 45;
  return(true);
}



//---------------------
void CheckSerial(void) {
  char casun[5];
  char c;
  
  if (Serial.available() >  0 ) {
    c = Serial.peek();  
    
    if (Serial.available() >=  TIME_MSG_LEN ) {
      casun[0]= c;  
      if (casun[0] ==  TIME_HEADER) {
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
       if (c == 'r') {
         c = Serial.read();
         Serial.print(c);
         if (c == 's') {
           EthernetInit(true);
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
   Serial.flush(); 
  }  // if (Serial.available() >  0 ) 
}  


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

#endif
