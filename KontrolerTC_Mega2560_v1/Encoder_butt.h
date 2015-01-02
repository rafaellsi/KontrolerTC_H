//Preveri:
//http://www.gammon.com.au/interrupts
//
//
#ifndef Encoder_butt_h
#define Encoder_butt_h

#include <Arduino.h>


extern void IzpisNaLCD(void);
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


// -----------------------------------------------------------------------------
// Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
/*
void isr(void)  {
    if (!digitalRead(ENC_DT_PIN)) {
        virtualPosition = virtualPosition + 1;
        
    }
    else {
        virtualPosition = virtualPosition - 1;
 //       Beep(0);
    }
} // isr
*/

boolean A_set = false;
boolean B_set = false;
boolean rotating=false;

// ------------------
void doEncoderA() {

   if (rotating) 
     delay (1);  // wait a little until the bouncing is done
  
  if (digitalRead(ENC_PIN_A) != A_set) {  // Test transition, did things really change? // debounce once more
    A_set = !A_set;
    if (A_set && !B_set)  // adjust counter + if A leads B
      virtualPosition += 1;
    rotating = false;  // no more debouncing until loop() hits again
  }
}

// ------------------
void doEncoderB() {

  if (rotating) 
     delay (1);  // wait a little until the bouncing is done
  
  if(digitalRead(ENC_PIN_B) != B_set ) {  // debounce once more
    B_set = !B_set;
    if ( B_set && !A_set )  // adjust counter + if A leads B
      virtualPosition -= 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// -----------------------------------------------------------------------------

void Encoder_init(void) {

    pinMode(ENC_CLK_PIN,INPUT_PULLUP);
    pinMode(ENC_DT_PIN, INPUT_PULLUP);
    pinMode(ENC_SW_PIN, INPUT_PULLUP);
    
    Serial.print(F("Encoder:"));
    attachInterrupt(0, doEncoderA, CHANGE);   // interrupt 0 is always connected to pin 2 on Arduino UNO
    attachInterrupt(1, doEncoderB, CHANGE);   // interrupt 1 is always connected to pin 3 on Arduino UNO
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
  
  rotating = true;
  
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
      Serial.print(F("Button pressed"));
      buttonPressed = true;
//      }  
    }
    
//    delay(2);                      // debounce       
  }
  prevSwState = lastSwState;
  
  if (virtualPosition != lastCount) {
//    infoModeLCD += (virtualPosition - lastCount);
    
    if (virtualPosition > lastCount)
      menuZaslonNum++;
    else
      menuZaslonNum--;
      
    lastCount = virtualPosition;  

    
    
    
    
    IzpisiNaLCD();
    Beep(10);
    osvetlitevLCD = 255;
    osvetLCD_start = now();
    
//    IzpisiNaLCD();
    Serial.println(F(""));
    Serial.print("Count");
    Serial.print(virtualPosition);
    Serial.print(" Menu");
    Serial.print(menuZaslonNum);
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
