#ifndef Izpis_LCD_h
#define Izpis_LCD_h

//#include <LiquidCrystal.h>

void LCDInitializacija(void);
void Beep(unsigned char delayms);
void IzpisHex2(int num);
void PrintDigitsLCDA(int digits);
void IzpisiNaLCD(void);
void IzpisControlMenu(void);
void IzpisInfoMenu(int infoLCD);







//LiquidCrystal lcdA(5, 9, 2, 3, 6, 10);
//LiquidCrystal lcdA(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // nano, glej zgoraj

#define MAX_LCD_OSV 160
//--------------------------------------------------------------------------------
void LCDInitializacija(void) {
  
  lcdA.begin(20, 2);
  delay(250);
  lcdA.print(F(" LCD OK"));
  analogWrite(LCD_OSW_SW, MAX_LCD_OSV);
  
}

//--------------------------------------------------------------------------------------------
void Beep(unsigned char delayms) {
  digitalWrite(BEEP_PIN, HIGH);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  digitalWrite(BEEP_PIN, LOW);       // 0 turns it off  
}  
//--------------------------------------------------------------------------------
void IzpisHex2(int num) {
  if (num < 0x10)
    lcdA.print(F("0"));  
  lcdA.print(num,HEX);
}
//--------------------------------------------------------------------------------
// izpais vodilni nicel za stevila manjsa od 10, za dvomestni izpis na LCDA
void PrintDigitsLCDA(int digits)
{
  if(digits < 10)
    lcdA.print(F("0"));
  lcdA.print(digits);
}

//--------------------------------------------------------------------------------


extern float TempVklopa(void);
extern float TempIzklopa(void);


extern byte prevTCState;
extern unsigned long onTimeTC;
extern unsigned long lastTCStateChg;
//extern int infoErr;
extern char infoErr[];

int infoModeLCD = -1;
int menuZaslonNum = -1;

boolean buttonPressed = false;
//--------------------------------------------------------------------------------
void IzpisiNaLCD(void) {
  
  static unsigned long t_lastNonDefaultLCDScreen;
  static int prevMenuZaslonNum;
  
  int menuZaslonNumMax[] = {7, 1};

  static int menuType = 0;
  int maxMenuType = 1;
  
  
  if (buttonPressed == true) {
//    if (menuType == 0) {
      menuType += 1;  
      menuZaslonNum = 0;
//      buttonPressed = false;
      if (menuType > maxMenuType) {
        menuType = maxMenuType;
      }
//    }
//    else {
//      buttonPressed = false;      
//    }  
  }
  
  if (menuZaslonNum != prevMenuZaslonNum || buttonPressed) {
    t_lastNonDefaultLCDScreen = now();
    prevMenuZaslonNum = menuZaslonNum;
    buttonPressed = false; 
  }
  if (menuZaslonNum != 0 || menuType != 0) {
    if (now() - t_lastNonDefaultLCDScreen > 30 || t_lastNonDefaultLCDScreen > now()) {
      menuType = 0;
      menuZaslonNum = 0;  
    }     
  }
  
  
  if (menuZaslonNum > menuZaslonNumMax[menuType]) {
      menuZaslonNum = menuZaslonNumMax[menuType];
//      virtualPosition = 20;
    }
    else if (menuZaslonNum < 0) {
      menuZaslonNum = 0;
//      virtualPosition = 0;
    } 

  switch(menuType) {
    case 0:
      IzpisInfoMenu(menuZaslonNum);
    break;
    case 1:
      IzpisControlMenu();
      menuType = 0;
    break;
    default:
      IzpisInfoMenu(menuZaslonNum);
    break;    
  }  
}  

//--------------------------------------------------------------------------------
void IzpisControlMenu(void) {
  lcdA.clear();
  lcdA.print(F("Se pride"));  
}  

extern int coRawVal;
//--------------------------------------------------------------------------------
void IzpisInfoMenu(int infoLCD)
{
  char cas[9];
  unsigned int addrTmp;
  static int pInfoLCD = -1;
  
//  lcdA.setCursor(0, 0);
  if (pInfoLCD != infoLCD) {
    lcdA.clear();
  }
      
  switch (infoLCD) {
    case 0:
    case 1:
       lcdA.setCursor(0, 0);
       PrintDigitsLCDA(day());
       if (pInfoLCD != infoLCD)
          lcdA.print(pika_str);
          
       lcdA.setCursor(3, 0);   
       PrintDigitsLCDA(month());
       if (pInfoLCD != infoLCD)
          lcdA.print(pika_str);
       
       lcdA.setCursor(6, 0);     
       lcdA.print(year()); 
       lcdA.setCursor(11, 0);
       NarediTimeStr(cas, now(), true);
       lcdA.print(cas);

       
       lcdA.setCursor(0, 1);
       lcdA.print(cTemperatura[OKOLICA_0], 1);
       if (pInfoLCD != infoLCD) {
          lcdA.setCursor(5, 1);
          lcdA.print(F("C RH:"));
       }
       
       lcdA.setCursor(10, 1);     
       lcdA.print(cVlaznost[0], 0);
       if (pInfoLCD != infoLCD)
          lcdA.print(F("%"));
       
       lcdA.setCursor(14, 1);   
       lcdA.print(cTemperatura[CRPALKA_0], 1);
       if (pInfoLCD != infoLCD)
          lcdA.print(deg_str); 
    break;
    
    case 2:
      lcdA.setCursor(0, 0);
      if (pInfoLCD != infoLCD) {
        lcdA.print(sensorIme[CRPALKA_0]);
        lcdA.print(separ_str);
      }
      lcdA.setCursor(7, 0);
      lcdA.print(cTemperatura[CRPALKA_0], 2);
      
      if (pInfoLCD != infoLCD)
          lcdA.print(space_str);
          
      lcdA.setCursor(13, 0);
      if (minute() < 59) {
         addrTmp = (unsigned int) (minute()+1) * sizeof(u2);
       }
       else {
         addrTmp = (unsigned int) 0;
       }
       addrTmp += addrLastHourTemp; 
       i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2)); 
       lcdA.print(cTemperatura[CRPALKA_0] - PretvotiEETemp2Float(u2.uival), 2);
      
      lcdA.setCursor(0, 1);
      if (releState_ventKompTC == R_TC_VENT_ON) {
        lcdA.print(F("V"));
      }
      if (stateTC == STATE_TC_ON) {
        if (releState_kompTC == R_TC_KOMP_ON) {
          lcdA.setCursor(1,1);
          lcdA.print(F("K"));
        }    
        if (releState_egrelecTC == R_TC_EGREL_ON) {
          lcdA.setCursor(2,1);
          lcdA.print(F("EG"));
        }  
      }
      else {
        lcdA.setCursor(2,1);
        lcdA.print(F("OFF"));
      }
      
      lcdA.setCursor(5,1);
      if (prevTCState == 0)
         lcdA.print(AutoTimeUnitConv(onTimeTC, cas),2);
       else
         lcdA.print(AutoTimeUnitConv(onTimeTC + now() - lastTCStateChg, cas),2);
      lcdA.print(cas);
      
      if (pInfoLCD != infoLCD) {
        lcdA.setCursor(13,1);
        lcdA.print(F("CO:"));
      }  
      lcdA.setCursor(17,1);
      lcdA.print(coRawValRef);
    break;  
    
    case 3:
       lcdA.setCursor(0, 0); 
       lcdA.print(sensorIme[PEC_PV]);
       lcdA.print(separ_str); 
       lcdA.print(cTemperatura[PEC_PV], 2);
       lcdA.print(deg_str);
       lcdA.setCursor(0, 1);
       lcdA.print(sensorIme[PEC_DV]);
       lcdA.print(separ_str);
       lcdA.print(cTemperatura[PEC_DV], 2);
       lcdA.print(deg_str);
    break;
    
    case 4:
       lcdA.setCursor(0, 0);
       lcdA.print(sensorIme[RAD_PV]);
       lcdA.print(separ_str); 
       lcdA.print(cTemperatura[RAD_PV], 2);
       lcdA.print(deg_str);
       lcdA.setCursor(0, 1);
       lcdA.print(sensorIme[RAD_DV]);
       lcdA.print(separ_str);
       lcdA.print(cTemperatura[RAD_DV], 2);
       lcdA.print(deg_str);
    break;
    
    case 5:
       lcdA.setCursor(0, 0);
       lcdA.print(sensorIme[PEC_TC_DV]);
       lcdA.print(separ_str); 
       lcdA.print(cTemperatura[PEC_TC_DV], 2);
       lcdA.print(deg_str);
       lcdA.setCursor(0, 1);
       lcdA.print(sensorIme[8]);
       lcdA.print(separ_str);
       lcdA.print(cTemperatura[8], 2);
       lcdA.print(deg_str);
    break;
    
     case 6:
      lcdA.setCursor(0, 0);
      lcdA.print(F("Tok:"));
      lcdA.setCursor(4, 0);
      lcdA.print(AC_mimax(false, true), 3);
      lcdA.print(F("A"));
      
      lcdA.setCursor(11, 0);
/*
      if (stateTC == TC_ON) {
        if (prevTCState == 1) 
          lcdA.print(F("ON"));
        else
          lcdA.print(F("SB"));
      }
      else
        lcdA.print(F("OFF"));
 */     
      if (releState_ventKompTC == R_TC_VENT_ON)
        lcdA.print(volt_str);
  
      if (stateTC == STATE_TC_ON) {
        if (releState_kompTC == R_TC_KOMP_ON)
          lcdA.print(F("K"));  
        if (releState_egrelecTC == R_TC_EGREL_ON)
          lcdA.print(F("EG"));
      }
      else {
        lcdA.print(F("OFF"));
      }
      
      lcdA.setCursor(15,0);
      if (prevTCState == 0)
         lcdA.print(AutoTimeUnitConv(onTimeTC, cas),2);
       else
         lcdA.print(AutoTimeUnitConv(onTimeTC + now() - lastTCStateChg, cas),2);
      lcdA.print(cas);
      
      lcdA.setCursor(1,1);
      lcdA.print(F("Tv "));
      lcdA.print(TempVklopa(), 1);  
      lcdA.setCursor(10, 1);
      lcdA.print(F("Ti "));
      lcdA.print(TempIzklopa(), 1);
    break;
    case 7:
      lcdA.setCursor(0, 0);
      lcdA.print(F("Zadnje pregrevanje: "));
      NarediTimeStr(cas, lastPregrevTime, false);
      lcdA.setCursor(0, 1);
      lcdA.print(day(lastPregrevTime));
      lcdA.print(pika_str);
      lcdA.print(month(lastPregrevTime));
      lcdA.print(pika_str);
      lcdA.print(year(lastPregrevTime)); 
      lcdA.print(space_str);
      lcdA.print(cas);
    break;  
    default:
      lcdA.setCursor(0, 0);
      lcdA.print(F("xXxXxXxX"));
      lcdA.setCursor(0, 1);
      lcdA.print(F("xXxXxXxX"));
      menuZaslonNum = 0;
    break;
  }
  pInfoLCD = infoLCD;    
}



/*
//  staro 16x1 (8x2)
//--------------------------------------------------------------------------------
void IzpisiNaLCD()
{
  char cas[5];
  
  
  lcdA.setCursor(0, 0);
  lcdA.clear();
      
  switch (infoModeLCD) {
    case 0:
       PrintDigitsLCDA(day());
       lcdA.print(F("."));
       PrintDigitsLCDA(month());
       lcdA.print(F("."));
       PrintDigitsLCDA(year()-2000); 
       lcdA.setCursor(2, 1);
       NarediTimeStr(cas, now(), false);
       lcdA.print(cas);
       
       infoModeLCD=10;
    break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
      lcdA.print(F("T"));
      lcdA.print(infoModeLCD-10+1);
      lcdA.print(F(":"));
      lcdA.setCursor(3, 0);
      lcdA.print(cTemperatura[infoModeLCD-10], 2);

      if(strlen(infoErr) > 0) {
        lcdA.setCursor(0, 1);
        lcdA.print(infoErr);
      }
//      else if (infoModeLCD-10 == crpalka_0){ 
//        lcdA.setCursor(2, 1);
//        lcdA.print(AvgVal(sumTemp[infoModeLCD-10], histLen*100.0));
//      }
        
      if (infoModeLCD-10 >= numSens -1)
        infoModeLCD = 18;
      else
        infoModeLCD ++;  
    break;
    case 18:
      lcdA.print(F("Tv "));
      lcdA.print(TempVklopa(), 1);  
      lcdA.setCursor(0, 1);
      lcdA.print(F("Ti "));
      lcdA.print(TempIzklopa(), 1); 
      infoModeLCD = 20;  
    break;
    case 20:
      lcdA.print(F("Tok:"));
      lcdA.setCursor(0, 1);
      lcdA.print(AC_mimax(), 3);
      lcdA.print(F("A"));
      infoModeLCD = 30;
    break;
    case 30:
      lcdA.setCursor(3, 0);
      if (releState_1 == R_TC_ON) {
        if (prevTCState == 1) 
          lcdA.print(F("ON"));
        else
          lcdA.print(F("SB"));
      }
      else
        lcdA.print(F("OFF"));
    
      lcdA.setCursor(0,1);
      if (prevTCState == 0)
         lcdA.print(AutoTimeUnitConv(onTimeTC, cas),2);
       else
         lcdA.print(AutoTimeUnitConv(onTimeTC + now() - lastTCStateChg, cas),2);
      lcdA.print(cas);
      infoModeLCD = 0;
    break;  
    default:
      lcdA.print(F("xXxXxXxX"));
      lcdA.setCursor(0, 1);
      lcdA.print(F("xXxXxXxX"));
      infoModeLCD = 0;
    break;
  }    
}

*/
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------



#endif
