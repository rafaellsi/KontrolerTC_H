#ifndef Kontr_TC_izvrs_h
#define Kontr_TC_izvrs_h

extern void ZapisiOnOffSD(int state, byte tipSpremembe);


void PreveriNapetosti(boolean internal, boolean external, boolean battery);
void PreklopiCrpalkoRad(byte newState);
void PreklopiVentilTCPec(byte newState);
void PreklopiCrpalkoTC(byte newState);
void ResetCrpTCVzr(void);

byte prevCrpTCState = 0;
byte prevVentTCState = 255;

unsigned long lastVentTCChg[2];
int preklopCrpTCVzr = 0;
unsigned long lastCrpTCStateChg;
//--------------------------------------------------------------------------------
void PreklopiCrpalkoRad(byte newState)
{
  char cas[13];
  prevCrpRadState = newState;
  lastCrpRadStateChg = now();
  
  if (newState == 1) {
    digitalWrite(RELE_CRAD, R_CRAD_ON);
    
    ZapisiOnOffSD(1, 10);
    Serial.println(F(" "));
    NarediTimeStr(cas, now());
    Serial.print(cas);
    Serial.print(F(" Vklop crp. radiator"));
  }
  else {
    digitalWrite(RELE_CRAD, R_CRAD_OFF);
    
    ZapisiOnOffSD(0, 10);
    Serial.println(F(" "));
    NarediTimeStr(cas, now());
    Serial.print(cas);
    Serial.print(F(" Izkolp crp. radiator"));
  }
}


//--------------------------------------------------------------------------------
void PreklopiVentilTCPec(byte newState)
{
  char cas[13];
  static byte lastCommand;
  
  if (newState < 2) {
    lastCommand = newState;
    delay(10);
    return;
  }  
  
  if (lastCommand < 2) {
    newState = lastCommand;
  }
  else {
    return;
  }  
  
  if (prevVentTCState == newState) {
    return;
  }
  
  if (newState < 2) {
    prevVentTCState = newState;
    lastVentTCChg[(int) newState] = now();
    if (newState == 1) {
      digitalWrite(VENTTC_2, LOW);
      digitalWrite(VENTTC_1, HIGH);
      
      Serial.println(F(""));
//      PreveriNapetosti(true, true, false);
      ZapisiOnOffSD(1, 1);

      Serial.println(F(" "));
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(" ");
      Serial.print(millis());
      Serial.print(F(" Odpiram ventil "));

    }
    else if (newState == 0){
      digitalWrite(VENTTC_1, LOW);
      digitalWrite(VENTTC_2, HIGH);
      
      ZapisiOnOffSD(0, 1);
      
      Serial.println(F(" "));
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(" ");
      Serial.print(millis());
//      PreveriNapetosti(true, true, false);
      Serial.print(F(" Zapiram ventil"));
    }
  }
}


//--------------------------------------------------------------------------------
void PreklopiCrpalkoTC(byte newState)
{
  char cas[13];
  
//  if (newState < 255) {
    
    if (newState == 1) {
      if (preklopCrpTCVzr == 0) {
        if (cTemperatura[PEC_TC_DV] >= tempVklopaCrpTC || cTemperatura[PEC_DV] >= tempVklopaCrpTC) {
          if ((cTemperatura[PEC_PV] < tempVklopaCrpTC - 5.0*dTemp) && (cTemperatura[RAD_PV] < tempVklopaCrpTC - 5.0*dTemp)) {
            return;
          }  
        }
      }  
    }
    Serial.println("");
    NarediTimeStr(cas, now());
    Serial.print(cas);
    
    if (newState == 1) {
      digitalWrite(RELE_CTC, R_CTC_ON);
      
      ZapisiOnOffSD(1, 2);
      
      Serial.print(F(" Vklop crpalke "));    
    }
    else {
      digitalWrite(RELE_CTC, R_CTC_OFF);
      
      ZapisiOnOffSD(0, 2);

      Serial.print(F(" Izklop crpalke"));
      
      ResetCrpTCVzr();
    }
    prevCrpTCState = newState;
    lastCrpTCStateChg = now();
//  }
}

//--------------------------------------------------------------------------------
void ResetCrpTCVzr(void)
{
  preklopCrpTCVzr = 0; 
}


#endif
