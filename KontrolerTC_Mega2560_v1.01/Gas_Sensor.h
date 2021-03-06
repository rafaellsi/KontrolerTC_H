#ifndef Gas_Sensor_h
#define Gas_Sensor_h


void Initilizacija_CO(void);
void PreveriCO_Senzor();

//--------------------------------------------------------------------------------------------
void Initilizacija_CO(void) {
  //#define CO_INIT
  
  #define CO_HEATING_STATE_OFF 0
  #define CO_HEATING_STATE_HIGH  1
  #define CO_HEATING_STATE_LOW  2
  #define CO_HEATING_OFF  HIGH
  #define CO_HEATING_ON  LOW
  #define CO_HEATING_PWM_LOW 184   // cca. 1.4V
  
  pinMode(CO_PWR_PIN, OUTPUT);
  pinMode(CO_DOUT_PIN, INPUT);  
}  

//--------------------------------------------------------------------------------------------

  unsigned long coRawValSum = 0;
  unsigned long numMerCO=0;
  int coRawValMax;
  int coRawValRef = 0;
//  float maxVoltGas = -999.9;
//  float minVoltGas = 999.9;
//------------------
void PreveriCO_Senzor() {

  static unsigned long t_CO_nextHChange = 0;
  static byte co_sens_heat_level = CO_HEATING_STATE_OFF;
  static unsigned long lastGasVoltAlert;
  static unsigned long lastGasAlert;
  // biriši ko rešiš napajanje
/*  float voltGas;
  voltGas = VoltageDivider(SENS_V5_3, v5_3_r1, v5_3_r2);
  if (voltGas > maxVoltGas)
    maxVoltGas = voltGas;
  else if (voltGas < minVoltGas)
    minVoltGas = voltGas;  
*/    
  float voltGas;
  voltGas = napetost_int[3][0];
  if (voltGas > 5.80) {
    digitalWrite(CO_PWR_PIN, HIGH);
    if (now() - lastGasVoltAlert > 10) {
      Serial.print(F("5V3(Err): "));
      Serial.println(voltGas);
      lastGasVoltAlert = now();
      coRawVal = 1024;
    }
    if (co_sens_heat_level != CO_HEATING_STATE_LOW) {
      analogWrite(CO_PWR_PIN, CO_HEATING_PWM_LOW);
      t_CO_nextHChange = now() + 90;
      co_sens_heat_level = CO_HEATING_STATE_OFF; 
    }
  } 
  else if (now() > t_CO_nextHChange) {
    if (co_sens_heat_level == CO_HEATING_STATE_LOW || co_sens_heat_level == CO_HEATING_STATE_OFF) { 
      t_CO_nextHChange = now() + 60;
//      noInterrupts();
      coRawVal = analogRead(CO_SENS_APIN);
//      interrupts();
//      coRawValSum += ((unsigned long) coRawVal);
//      numMerCO+= 1;
      
      digitalWrite(CO_PWR_PIN, LOW);     
      
      co_sens_heat_level = CO_HEATING_STATE_HIGH;
      Serial.print(F(">"));
    }  
    else if (co_sens_heat_level == CO_HEATING_STATE_HIGH) {
//      noInterrupts();
      coRawVal = analogRead(CO_SENS_APIN);
      coRawValRef = coRawVal;
//      interrupts();
//      coRawValSum += ((unsigned long) coRawVal);
//      numMerCO+=1;
//      
      analogWrite(CO_PWR_PIN, CO_HEATING_PWM_LOW);
      t_CO_nextHChange = now() + 90;
      co_sens_heat_level = CO_HEATING_STATE_LOW;
      Serial.print(F("<"));
    }   
  }
  else {
//    noInterrupts();
    coRawVal = analogRead(CO_SENS_APIN);
//    interrupts();
    
  }
  
  coRawValSum += ((unsigned long) coRawVal);
  numMerCO++;
  if (coRawVal > coRawValMax) {
    coRawValMax = coRawVal;
  } 
  
  int coRawAlertVal = 10;
  if (coRawValRef > coRawAlertVal) {
    if (co_sens_heat_level != CO_HEATING_STATE_OFF) {
      if (digitalRead(CO_DOUT_PIN) == HIGH) {
        unsigned long gasAlertInterval = map((unsigned long) coRawValRef, 0UL, 1023UL, 150000UL, 0UL); 
        if (millis() - lastGasAlert > gasAlertInterval || millis() < lastGasAlert) {
          Serial.print(F("CO! "));
          lastGasAlert = millis();
          Beep(map(coRawValRef, coRawAlertVal, 1023, 10, 50));
 //         Beep(50);
        }
      //      
      }
    }
  }  
}  


//--------------------------------------------------------------------------------------------
#endif 
