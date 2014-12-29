#ifndef Tok_napetost_h
#define Tok_napetost_h

extern boolean releState_1;
//extern float vccInternal;
//extern int midR;

static long readVcc();
static float PretvoriVAmp5A(int sensVal);

static int midR = 512;
static float vccInternal;
//--------------------------------------------------------------------------------
// branje senzorja toka, pretvorba, izracun efektivne vrednosti toka
static float AC_mimax(boolean izpis = false, boolean forceCalc = false) {
  int val_I;                      // prebrana vrednost senzorja
  int maxR, minR;                 // min/max prebrana vrednost senzorja 
  unsigned long startTime;        // zacetek meritve 
  unsigned long  dTRSt;           // cas zadnjega branja senzorja
  unsigned long  dTRStMin = 200;  // minimalni cas med dvema branjema senzorja - v microsecundah

  int numMerAC = 1;
  float efI;  
  float tok01;
  float vccFactor = 1.0;
  
  if (releState_1 == R_TC_OFF && forceCalc == false)
    return(0.0);
  
  
  
  #ifdef INTFACT
  vccFactor = vccInternal/5.0;
  #endif
  
  
  delay(2);
  startTime = millis();
  
  dTRSt = micros();
  noInterrupts();
  val_I = analogRead(SENS_TOK) *vccFactor;
  interrupts();
  tok01 = PretvoriVAmp5A(val_I);
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
    noInterrupts();
    val_I = analogRead(SENS_TOK)* vccFactor;
    interrupts();
    
    tok01 = PretvoriVAmp5A(val_I);
    
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
  return (efI);
} 

//--------------------------------------------------------------------------------
// pretvorba prebrane vrednosti senzorj v tok
static float PretvoriVAmp5A(int sensVal) {
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
}  

//--------------------------------------------------------------------------------------------
float VoltageDivider(int analRead, float r1, float r2) {
  
  int numSamples = 5;
  float napetost = 0.0;
  
  noInterrupts();
  for (int i = 0; i < numSamples; i++) {
    delay(2);
    napetost += (analogRead(analRead) * vccInternal/ 1024.0) * (r1 + r2) / r2;
  }
  interrupts();
  napetost /= ((float) numSamples);
  return (napetost);
}  

//--------------------------------------------------------------------------------------------
void PreveriNapetosti(boolean internal = false, boolean external = false, boolean battery = false)
{
  static float v12_r1 = 6800;
  static float v12_r2 = 3300;
  static float v5_r1 =  1000;
  static float v5_r2 =  3900;
    
  if (internal) {
    vccInternal = readVcc()/1000.0;
    delay(20);
  }
    
  if (external) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
    }   
    Serial.print(F(" 12V: "));
    delay(5);
    Serial.print(VoltageDivider(SENS_V12, v12_r1, v12_r2));
    Serial.print(F("  5V2: "));
    delay(5);
    Serial.print(VoltageDivider(SENS_V5_2, v5_r1, v5_r2)); 
  }
  if (battery) {
    if (vccInternal == 0) {
      vccInternal = 5.0;
    }   
    Serial.print(F(" Batt+: "));
    delay(5);
    Serial.print(analogRead(SENS_RTC_BATT) * vccInternal/ 1024.0);
  }
}

//--------------------------------------------------------------------------------------------
static long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  analogReference(DEFAULT);
  return result; // Vcc in millivolts
}


#endif
