#ifndef Gas_Sensor_h
#define Kontroler_TC_h

//--------------------------------------------------------------------------------------------
void Initilizacija_CO(void) {
  //#define CO_INIT
  
  #define CO_HEATING_STATE_OFF 0
  #define CO_HEATING_STATE_HIGH  1
  #define CO_HEATING_STATE_LOW  2
  #define CO_HEATING_OFF  HIGH
  #define CO_HEATING_ON  LOW
  #define CO_HEATING_PWM_LOW 184
  
  pinMode(CO_PWR_PIN, OUTPUT);
  pinMode(CO_DOUT_PIN, INPUT);  
}  

//--------------------------------------------------------------------------------------------




#ifdef CO_INIT
  int numMerCO=0;
#endif


//------------------
void PreveriCO_Senzor() {
  static unsigned long t_CO_timer = 0;
  static unsigned long t_CO_nextHChange = 0;
  static byte co_sens_heat_level = CO_HEATING_STATE_OFF;

  
  if (now() > t_CO_nextHChange) {
    if (co_sens_heat_level == CO_HEATING_STATE_LOW || co_sens_heat_level == CO_HEATING_STATE_OFF) {


#ifdef CO_INIT
      t_CO_nextHChange = now() + 175000;
#else      
      t_CO_nextHChange = now() + 60;
      noInterrupts();
      coRawVal = analogRead(CO_SENS_APIN);
      interrupts();
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

      
      noInterrupts();
      coRawVal = analogRead(CO_SENS_APIN);
      interrupts();

      
      analogWrite(CO_PWR_PIN, CO_HEATING_PWM_LOW);
      t_CO_nextHChange = now() + 90;
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
  noInterrupts();
  coRawVal += analogRead(CO_SENS_APIN);
  interrupts();
  numMerCO++;
#endif
}  

#endif 
