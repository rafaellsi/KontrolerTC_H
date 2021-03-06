
#ifndef Tok_napetost_h
#define Tok_napetost_h

//extern boolean releState_TC;
//extern float vccInternal;
//extern int midR;

float AC_mimax(boolean izpis, boolean forceCalc);
float Tok_12V(void);
float PretvoriV2A_asc712_DC(int sensVal);
float PretvoriV2A_asc712(int sensVal);
float VoltageDivider(int analRead, float r1, float r2, float korFact);
void PreveriNapetosti(boolean internal, boolean external, boolean battery);
long readVcc(void);




int midR = 512;

//--------------------------------------------------------------------------------
// branje senzorja toka, pretvorba, izracun efektivne vrednosti toka
float AC_mimax(boolean izpis = false, boolean forceCalc = false) {
  
  static unsigned long lastMerPorabeWH = millis();
  int val_I;                      // prebrana vrednost senzorja
  int maxR, minR;                 // min/max prebrana vrednost senzorja 
  unsigned long startTime;        // zacetek meritve 
  unsigned long  dTRSt;           // cas zadnjega branja senzorja
  unsigned long  dTRStMin = 200;  // minimalni cas med dvema branjema senzorja - v microsecundah

  unsigned long timeDiff;
  
  int numMerAC = 1;
  float efI;  
  float tok01;
  float vccFactor = 1.0;
  
  if (stateTC == STATE_TC_OFF && forceCalc == false)
    return(0.0);
  
  
  
  #ifdef INTFACT
  vccFactor = vccInternal/5.0;
  #endif
  
  
  delay(2);
  startTime = millis();
  
  dTRSt = micros();
//  noInterrupts();
  val_I = analogRead(SENS_TOK) *vccFactor;
//  interrupts();
  tok01 = PretvoriV2A_asc712(val_I);
  efI = (tok01 * tok01);
  
  maxR = val_I;
  minR = val_I;
  
  do {
   while (micros() - dTRSt < dTRStMin) {
//       delayMicroseconds(1);
    }

//    if (micros() - dTRSt < dTRStMin)
//      delayMicroseconds((dTRStMin+5) - (micros() - dTRSt));

    dTRSt = micros();
//    noInterrupts();
    val_I = analogRead(SENS_TOK)* vccFactor;
//    interrupts();
    
    tok01 = PretvoriV2A_asc712(val_I);
    
    efI += ((double) tok01 * (double) tok01);
    
    if (val_I > maxR) {
      maxR = val_I;
    }  
    else if (val_I < minR) {
      minR = val_I; 
    }
    
    numMerAC++;
  }  while (millis() - startTime < 20 && millis() >= startTime);
  
  
  
  efI = efI/((float) numMerAC);
  efI = sqrt(efI);


  if (izpis) { 
    Serial.print(F("Min.-max:"));
    Serial.print(minR);
//    Serial.print(" (");

//    Serial.print(PretvoriVAmp5A(minR));
//    Serial.print("A) -");
    Serial.print(F("-"));
    Serial.print(maxR);
//    Serial.print(" (");

//    Serial.print(PretvoriVAmp5A(maxR));
//    Serial.print("A) -");

    Serial.print(F(":"));
//    Serial.print(" midR:");
  
    Serial.print(midR);
    Serial.print(F(":"));
    
    Serial.print(vccInternal,2);
    Serial.print(F("V -"));
  }
  
//  poraba += (((now() - lastAMesTime)/3600.0) * efI);
//  lastAMesTime = now();
  midR = (3*midR + minR + maxR)/5;
  
  if (millis() >= lastMerPorabeWH) {
    timeDiff = millis() - lastMerPorabeWH; 
  }
  else {
    timeDiff = millis();
  }  
  lastMerPorabeWH = millis();
  porabaWH += (efI *(Vrms * ((float) timeDiff/1000.0)/3600.0));  // poraba v VAh - (timer [ms]/1000)[s] /3600 [h]
  
  return (efI);
} 



//--------------------------------------------------------------------------------
float Tok_12V(void) {
  
  int raw_I;
  float tok_12V;
  
  raw_I = analogRead(SENS_TOK_12V);
  
  tok_12V = PretvoriV2A_asc712_DC(raw_I);
  
  sumTok_12V += tok_12V;
  nMerTok_12V += 1;
  
  if (tok_12V > maxTok_12V)
    maxTok_12V = tok_12V;    
  
  return(tok_12V);
}  

//--------------------------------------------------------------------------------
float PretvoriV2A_asc712_DC(int sensVal) {
    float tok;  
    int cVal = sensVal - vccInternal/2;
    
    if (vccInternal > 0)
      tok = (((float) cVal / 1024.0)*vccInternal);
    else  
      tok = (((float) cVal / 1024.0)*5.0);
    
    tok -= (vccInternal/2.0); 
/*    
    if (vccInternal > 0)
      tok = (((float) cVal / 1024.0)*vccInternal);
    else  
      tok = (((float) cVal / 1024.0)*5.0);
//  tok = (((float) sensVal / 1024.0)*5.0) - 2.50;

*/
    return(tok /= 0.185);                              //za senzor (max. 5A) 0.185V na 1A 
//    return(tok /= 0.100);                              //za senzor (max. 20A) 0.100V na 1A
//      return(tok /= 0.001250);                        //test za 0.01V/A
}  

//--------------------------------------------------------------------------------
// pretvorba prebrane vrednosti senzorj v tok
float PretvoriV2A_asc712(int sensVal) {
    float tok;  
    int cVal = sensVal - midR;
    
    #ifdef INTFACT
      tok = (((float) cVal / 1024.0)*5.0);
    #else
      tok = (((float) cVal / 1024.0)*vccInternal);
    #endif  
/*    
    if (vccInternal > 0)
      tok = (((float) cVal / 1024.0)*vccInternal);
    else  
      tok = (((float) cVal / 1024.0)*5.0);
//  tok = (((float) sensVal / 1024.0)*5.0) - 2.50;

*/
//    return(tok /= 0.185);                              //za senzor (max. 5A) 0.185V na 1A 
    return(tok /= 0.100);                              //za senzor (max. 20A) 0.100V na 1A
//      return(tok /= 0.001250);                        //test za 0.01V/A
}  

//--------------------------------------------------------------------------------------------
float VoltageDivider(int analRead, float r1, float r2, float korFact = 1.0) {
  
  int numSamples = 5;
  float napetost = 0.0;
   
//  noInterrupts();
 
  for (int i = 0; i < numSamples; i++) {
      delay(2);
      napetost += (analogRead(analRead) * vccInternal/ 1023.0) * (r1 + r2) / r2;
  }
//  interrupts();
  napetost /= ((float) numSamples);
  return (napetost * korFact);
}  

//static float v5_3_r1 = 2200;
//static float v5_3_r2 = 2200;
/*
//--------------------------------------------------------------------------------------------
void PreveriNapetosti(boolean internal = false, boolean external = false, boolean battery = false)
{
  static float v12_r1 = 6800;
  static float v12_r2 = 3300;
  static float v5_r1 =  1000;
  static float v5_r2 =  3900;
  
  
  float vTemp;
  
  if (internal) {
    vTemp = (float) readVcc()/1000.0;
    Serial.print(F(" 5In:"));
    if (vTemp < 5.0 * 0.5 || vTemp > 1.5 *5.0) {
      Serial.print(vTemp);
      Serial.print(F("(Err) "));
    }
    else {  
      vccInternal = vTemp;
    }
    
    Serial.print(vccInternal);
    delay(5);
  }
    
  if (external) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
    }   
    Serial.print(F(" 12V:"));
    delay(5);
    Serial.print(VoltageDivider(SENS_V12, v12_r1, v12_r2, 1.01759));
    Serial.print(F("  5V2:"));
    delay(5);
    Serial.print(VoltageDivider(SENS_V5_2, v5_r1, v5_r2, 1.0041)); 
    Serial.print(F("  5V3:"));
    delay(5);
    Serial.print(VoltageDivider(SENS_V5_3, v5_3_r1, v5_3_r2)); 

  }
  if (battery) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
    }   
    Serial.print(F(" Batt+:"));
    delay(5);
    Serial.print(analogRead(SENS_RTC_BATT) * vccInternal/ 1024.0);
  }
}
*/

float napetost_int[5][3];
//--------------------------------------------------------------------------------------------
void napetostiMinMax(int n, boolean isFirstTime) {
    if (isFirstTime) {
      napetost_int[n][1] = napetost_int[n][0];
      napetost_int[n][2] = napetost_int[n][0];
    }
    else {
      if (napetost_int[n][0] < napetost_int[n][1])
        napetost_int[n][1] = napetost_int[n][0];
      else if (napetost_int[n][0] > napetost_int[n][2])
        napetost_int[n][2] = napetost_int[n][0];  
    }
}  

//--------------------------------------------------------------------------------------------
void PreveriNapetosti(boolean izpis = true, boolean internal = false, boolean external = false, boolean battery = false)
{
  
  static boolean isFirstTime = true;
/*  
  static float v12_r1 = 6800;
  static float v12_r2 = 3300;
  static float v5_r1 =  1000;
  static float v5_r2 =  3900;
*/  
  static float upr_delilnika[3][2] = {6800, 3300, 1000, 3900, 2200, 2200};
  
  float vTemp;
  
  if (internal) {
    napetost_int[0][0] = (float) readVcc()/1000.0;

    if (napetost_int[0][0] < 5.0 * 0.5 || napetost_int[0][0] > 1.5 *5.0) {
      Serial.print(F(" 5In:"));
      Serial.print(napetost_int[0][0]);
      Serial.print(F("(Err) "));
    }
    else {  
      vccInternal = napetost_int[0][0];
    }
    
    napetostiMinMax(0, isFirstTime);
    
    if (izpis) {
      Serial.print(F(" 5In:"));
      Serial.print(napetost_int[0][0]);
      Serial.print("(");
      Serial.print(napetost_int[0][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[0][2]);
      Serial.print(F(")"));
    }
    //delay(2);
  }
    
  if (external) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
      napetost_int[0][0] = vccInternal;
      napetost_int[0][1] = vccInternal;
      napetost_int[0][2] = vccInternal;
    }   
    
    
    napetost_int[1][0] = VoltageDivider(SENS_V12, upr_delilnika[0][0], upr_delilnika[0][1], 1.01759);
    napetostiMinMax(1, isFirstTime);
    if (izpis) {
      Serial.print(F(" 12V:"));
      Serial.print(napetost_int[1][0]);
      Serial.print(F("("));
      Serial.print(napetost_int[1][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[1][2]);
      Serial.print(F(")"));
    }
//    delay(5);
    napetost_int[2][0] = VoltageDivider(SENS_V5_2, upr_delilnika[1][0], upr_delilnika[1][1], 1.0041);
    napetostiMinMax(2, isFirstTime);
    if (izpis) {
      Serial.print(F(" 5V2:"));
      Serial.print(napetost_int[2][0]);
      Serial.print(F("("));
      Serial.print(napetost_int[2][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[2][2]);
      Serial.print(F(")"));
    }  
//    delay(5);
    napetost_int[3][0] = VoltageDivider(SENS_V5_3, upr_delilnika[2][0], upr_delilnika[2][1]);
    napetostiMinMax(3, isFirstTime);
    if (izpis) {
      Serial.print(F(" 5V3:"));
      Serial.print(napetost_int[3][0]);
      Serial.print(F("("));
      Serial.print(napetost_int[3][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[3][2]);
      Serial.print(F(")"));
    }
//    delay(5);
    napetost_int[4][0] = analogRead(SENS_3V3_SD) * vccInternal/ 1023.0;
    napetostiMinMax(4, isFirstTime);
    if (izpis) {
      Serial.print(F(" 3V3:"));
      Serial.print(napetost_int[4][0]);
      Serial.print(F("("));
      Serial.print(napetost_int[4][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[4][2]);
      Serial.print(F(")"));
    }
    
  }
  if (battery) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
    }   
    napetost_int[4][0] = analogRead(SENS_RTC_BATT) * vccInternal/ 1024.0;
    napetostiMinMax(4, isFirstTime);
    if (izpis) {
      Serial.print(F(" Batt+:"));
      Serial.print(napetost_int[4][0]);
      Serial.print(F("("));
      Serial.print(napetost_int[4][1]);
      Serial.print(F("/"));
      Serial.print(napetost_int[4][2]);
      Serial.print(F(")"));
    }
//    delay(5);
  }
  isFirstTime = false;
}


//--------------------------------------------------------------------------------------------
long readVcc(void) {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
//  delay(3); 
//  noInterrupts();
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
//  delay(2); // Wait for Vref to settle
  delay(2);
  
  ADCSRA |= _BV(ADSC); // Start conversion
//  while (bit_is_set(ADCSRA,ADSC)); // measuring
  
  while (bit_is_set(ADCSRA,ADSC)) { // measuring
  } 
  
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
//  interrupts();
  long result = (high<<8) | low;
  
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000


//  analogReference(DEFAULT);
 return result; // Vcc in millivolts

}


#endif
