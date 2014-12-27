//Preveri:
//http://www.gammon.com.au/interrupts
//
//
#ifndef Encoder_butt_h
#define Encoder_butt_h

#include <Arduino.h>

// -----------------------------------------------------------------------------
// constants



// -----------------------------------------------------------------------------
// global vars

volatile int virtualPosition    = 0;

// -----------------------------------------------------------------------------
// forward decls
/*
void isr();
void loop();
void setup();
*/

void Beep(unsigned char delayms){
  digitalWrite(BEEP_PIN, HIGH);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  digitalWrite(BEEP_PIN, LOW);       // 0 turns it off  
}  

// -----------------------------------------------------------------------------
// Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK

void isr(void)  {
    if (!digitalRead(ENC_DT_PIN)) {
        virtualPosition = virtualPosition + 1;
        
    }
    else {
        virtualPosition = virtualPosition - 1;
 //       Beep(0);
    }
} // isr

// -----------------------------------------------------------------------------

void Encoder_init(void) {

    pinMode(ENC_CLK_PIN,INPUT);
    pinMode(ENC_DT_PIN, INPUT);
    pinMode(ENC_SW_PIN, INPUT_PULLUP);
    
    Serial.print(F("Encoder:"));
    attachInterrupt(0, isr, FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
    Serial.println(F("OK"));
//    Serial.println("Start");

} // setup
static int osvetlitevLCD = 0;
// -----------------------------------------------------------------------------

void Encoder_check(void) {

  static byte prevSwState = LOW;
  byte lastSwState;
  static int lastCount = 0;
  static unsigned long osvetLCD_start;
    
  lastSwState = digitalRead(ENC_SW_PIN);
  
  if (lastSwState == LOW) {        // check if pushbutton is pressed
    if (prevSwState == HIGH) {
      virtualPosition = 0;            // if YES, then reset counter to ZERO
      osvetlitevLCD = 255;
      osvetLCD_start = now();
//     analogWrite(LCD_OSW_SW, osvetlitevLCD);
//    while (!digitalRead(ENC_SW_PIN)) {  // wait til switch is released
//    }
      Beep(25);
    }
    
//    delay(2);                      // debounce       
  }
  prevSwState = lastSwState;
  
  if (virtualPosition != lastCount) {
    modeLCD += (virtualPosition - lastCount);
    Beep(10);
    if (modeLCD > 20) {
      modeLCD = 20;
//      virtualPosition = 20;
    }
    else if (modeLCD < 0) {
      modeLCD = 0;
//      virtualPosition = 0;
    } 
    lastCount = virtualPosition;
    osvetlitevLCD = 255;
    osvetLCD_start = now();
    
    IzpisiNaLCD();
    
    Serial.print("Count");
    Serial.println(virtualPosition);
  }
  if (osvetlitevLCD > 0) {
    osvetlitevLCD = 255 + (osvetLCD_start - now()) * 2;
    osvetlitevLCD = constrain(osvetlitevLCD, 0, 255);
    analogWrite(LCD_OSW_SW, osvetlitevLCD);
  }  
} //loop
    
/*    
    void isr ()  {

    static unsigned long                lastInterruptTime = 0;

    unsigned long                       interruptTime = millis();

    // If interrupts come faster than 5ms, assume it's a bounce and ignore
    if (interruptTime - lastInterruptTime > 5) {
        if (!digitalRead(PinDT))
            virtualPosition = (virtualPosition + 1);
        else
            virtualPosition = virtualPosition - 1;
        if (virtualPosition > maxPosition)
            virtualPosition = maxPosition;
        if (virtualPosition < 0)
            virtualPosition = 0;
        }
    lastInterruptTime = interruptTime;
    } // ISR
    
    */


    
#endif 
