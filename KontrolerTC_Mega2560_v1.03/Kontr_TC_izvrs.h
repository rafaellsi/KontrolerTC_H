#ifndef Kontr_TC_izvrs_h
#define Kontr_TC_izvrs_h

extern void ZapisiOnOffSD(int state, byte tipSpremembe);



void PreklopiCrpalkoRad(byte newState);
void PreklopiVentilTCPec(byte newState);
void PreklopiCrpalkoTC(byte newState);
void ResetCrpTCVzr(void);

byte prevCrpTCState = STATE_CRP_TC_OFF;
byte prevVentTCState = STATE_VENT_TC_NDEF;

unsigned long lastVentTCChg[2];
int preklopCrpTCVzr = 0;        //0 - normal
                                //1 - 
                                //2 - 
                                //3 - 
                                //4 - 

//--------------------------------------------------------------------------------
void PreklopiCrpalkoRad(byte newState)
{
  char cas[13];
  
  
  if (newState == 1) {
    digitalWrite(RELE_CRAD, R_CRAD_ON);
    
    ZapisiOnOffSD(1, 10);
    Serial.println(space_str);
    NarediTimeStr(cas, now());
    Serial.print(cas);
    Serial.print(F(" Vklop crp. radiator"));
  }
  else {
    digitalWrite(RELE_CRAD, R_CRAD_OFF);
    
    onTimeCrpRad += (now() - lastCrpRadStateChg);
    
    ZapisiOnOffSD(0, 10);
    Serial.println(space_str);
    NarediTimeStr(cas, now());
    Serial.print(cas);
    Serial.print(F(" Izkolp crp. radiator"));
  }
  prevCrpRadState = newState;
  lastCrpRadStateChg = now();
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
    if (newState == STATE_VENT_TC_ON) {
      digitalWrite(VENTTC_2, LOW);
      digitalWrite(VENTTC_1, HIGH);
      
      Serial.println();
//      PreveriNapetosti(true, true, false);
      ZapisiOnOffSD(1, 1);

      Serial.println(space_str);
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(space_str);
      Serial.print(millis());
      Serial.print(F(" Odpiram ventil "));

    }
    else if (newState == STATE_VENT_TC_OFF){
      digitalWrite(VENTTC_1, LOW);
      digitalWrite(VENTTC_2, HIGH);
      
      ZapisiOnOffSD(0, 1);
      
      Serial.println(space_str);
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(space_str);
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
    
    if (newState == STATE_CRP_TC_ON) {
      if (preklopCrpTCVzr == 0) {
        if (cTemperatura[PEC_TC_DV] >= tempVklopaCrpTC || cTemperatura[PEC_DV] >= tempVklopaCrpTC) {
//Sprem. 23.12.2015          if ((cTemperatura[PEC_PV] < tempVklopaCrpTC - 5.0*dTemp) && (cTemperatura[RAD_PV] < tempVklopaCrpTC - 5.0*dTemp)) {
          if ((cTemperatura[PEC_PV] < 0.525*tempVklopaCrpTC) && (cTemperatura[RAD_PV] < 0.525*tempVklopaCrpTC)) {
            return;
          }  
        }
      }  
    }
    Serial.println();
    NarediTimeStr(cas, now());
    Serial.print(cas);
    
    if (newState == STATE_CRP_TC_ON) {
      digitalWrite(RELE_CTC, R_CTC_ON);
      
      
      ZapisiOnOffSD(1, 2);
      Serial.print(F(" Vklop crpalke TC"));    
    }
    else {
      digitalWrite(RELE_CTC, R_CTC_OFF);
      onTimeCrpTC += (now() - lastCrpTCStateChg);
      ZapisiOnOffSD(0, 2);

      Serial.print(F(" Izklop crpalke TC"));
      
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
