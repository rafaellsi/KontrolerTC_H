



// Kontroler_TC_Vx_xx.ino
// v0.02 - dodan zapis senzorjev v tabelo
// v0.03 - dodan RTC - DS1307
//     * Scl -> A5
//     * Sda -> A4
// v0.04 - spremenjen izpis na "Serial"
// v0.05 - dodan time Interval povprecje
// v0.06 - dodan rele
//       - dodan tok sens (ASC712)  
// v0.09a - osnovna Dallas knjiznica
// v0.10a - ciscenje programa
//        - test interna temp. in voltaza   
//        - dodana nastavitev casa
// v0.11a - dodan LCD
//  pin "chipSelectSD" prestavljen iz 8 na 10
// v0.13a - logika za tc
// v0.14a - ciscenje viska
// v0.20 - prvo testiranje na napravi
// v0.23 - sprememba "deltaTh" iz 8.0 na 6.45
// v0.27 - spremenjena "setup" procedura
//       - dodan podatek o FreeRAM
//       - odstranitev nekaterih nastavitev preko serial
//       - redefinicija pavze v "loop".... 
// v0.28 - skrajsanje skice - dodan "__SER_DEBUG__"
// v0.29
// v0.30 - shranjevanje temp v Eprom (AT24C32), izracun povprecja zanjh 24 ur

//Kontroler_TC_Mega2560_Vx_xx.ino
// v0.31 - prestavitev v Mego iz "Kontroler_TC_v0_31.ino"
// v0.32 - dodani senzorji na peci
//        - zamenjava tok senzorja (staro 5A, nov 20A)
//        - dodano RF oddajnik FS1000A - virtual wire
// v0.33 - iskanje tezave z prekinitvami
// v0.36 - dodano krmiljenje crplke ogrevanja vode iz peci
//       - dodano krmiljenje vetila povezave pec-TC
// v0.37 - dodano "nocno" prenos toplote iz peci v TC
// v0.38 - zniana temp. vklopa crpalke TC     iz 80 -> 75 (izlop 75 -> 70)
//       -  povisana temp stalno odprtega ventila 55 -> 60
//       - podaljsan cas min. delovanja  5->7 min 
//       - dodan zamik zapiranja ventila za izklopom crpalke
// v0.39 - dodan pogoj za vklop(izklop) pri nizki temp. cez dan na delavnik
// v0.40 - dodan temp. senzor na izhod na peci proti TC ("pec_TC_dv")
//       - dodano branje cevnega termostata
// v0.41 - nova funkcija za preverjanje statusa in preklope crpalke in ventila TC
//       - uporaba cevnega stikala
// v0.42 - odpravljanje tezav pri pogojih vklopa crpalke TC
// v0.43 - dodano obratovanja TC, kot pogoj za vklop crpalke "prevTCState == 1"
// v0.44 - spremenjene temperature vlopa crpalke TC (TempVklopaCrpTC_NTemp()), pri dnevnem delovanju (!IsNTempCas())
// v0.45 - dodano stikalo za upostevanje el. tarife pri vklopu TC "upostevajElTarife = true/false)
//       - spremenjeno iz 3 -> 2 minute "minCrpTCRunTimeMin"
// v0.46 - spremenejen zapis temeratur v EPROM (zgodovina za vse temp. za en dan ... prej ena temp. za sedem dni)
//       - addresa prvega zapisa iz 64 v 1024 ("addrTempBack")
//       - temperature v epromu zapisane kot int ((int) temp (eprom) = 100 * (float) temp. sensorja)
// v0.47 - preureditev relejev, dodan SSR
// v0.48 -  dodano odpiranje ventila po preteku "transfToplote"in ni cas "IsNTempCas()", ce je temp. TC visja od meje zapiranja ventila
//       - dodan pogoj minimalne temp. peci, pri ponovnem odpiranju ventila TC - "minTempPecPonovnoOdpVent"
//       - "minCrpRadRunTimeMin" 3 -> 7 min
// v0.49 - kontrola proti vrenju - crpalkaTC 
// v0.50 - dodana "maxTempDVPec = 80.01" // max. temperatura dviznega voda peci", 
//       - "isCrpRadAsAbsTemp = true; //upobi absol. temp za zagon crpalke",
//       - "float crpRadAsAbsTemp[24]"
// v0.51 - poskus dodajanja zun temp .... za izraun pogoja za vjlop crpalke
// v0.52 - dodano znizanje temp. TC, ob dolocenih pogojih -"preklopCrpTCVzr = 4"
// v0.54 - dzamenjava spremenljivke "upostevajElTarife" s funkcijo "UpostevajElTarife"
// v0.55 - dodan pogoj zagona rpalke radiatorjev v primeru napacnega krozenja vode
//       - imitacija kontrole CRC-ja DS senzorjev
// v0.56 - tedenska zgodovina temperatur (na vsako uro)
//  !!!!-zacasno izklopljene elektricne tarife ....
// v0.59 - nadgrajena izbira glede na tarife (pec - elektrika)
// v0.94 - sprememba izracuna hitrosti gretja
// v0.95 - dodana info hitrost gretja



#include <avr/pgmspace.h>
#include <Arduino.h>

/*
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
*/
//#include <math.h>

#include <OneWire.h>
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <EtherCard.h>
#include <DHT.h>

//#include <RHReliableDatagram.h>
//#include <RH_NRF24.h>
#include <SPI.h>

#include "Configuration.h"
#include "Eprom_external.h"

#include "Cas_funkcije.h"
#include "Kontr_TC_izvrs.h"
#include "Kontr_TC_spl.h"
#include "Kontr_TC.h"



#include "Tok_napetost.h"
#include "Temperature.h"

#include "LCD_funkcije.h"
#include "Encoder_butt.h"
#include "Zapisi_sd.h"
#include "Ethernet_funk.h"
#include "Gas_Sensor.h"


int prevMinute = -1;

unsigned long lastReleChg;    // cas zadnje spremembe stanja releja TC
unsigned long prevLCDizp;     // cas zadnjega izpisa na LCD

//--------------------------------------------------------------------------------
void setup(void)
{
  
  char infoSet[9];
  
  NastavitevPinov();
//  SPI.setClockDivider(SPI_CLOCK_DIV16); 
   
  Serial.begin(115200);
  Serial.flush();
  Serial.println(F(""));
  Serial.println(F("Kontroler TC"));
  Serial.println(VERSION);
  
  PreveriNapetosti(true, true, true, false);
  Serial.println(F(""));
  Serial.print(F("Tok: "));
  Serial.println(Tok_12V());
  
    //-------------------
  Wire.begin();
  
  //-------------------
  delay(2000);
  LCDInitializacija();
//  EthernetInit(false);
  TempSensorsInit(); 
  Initilizacija_CO();
  
  // Start up the RTC
  tmElements_t tm;
  if (RTC.read(tm)) {
  }
  else {
    if (RTC.chipPresent()) {
      Serial.println(F("The DS1307 je ustavljen. Prosim nastavi cas."));
      Serial.println();
    } else {
      Serial.println(F("DS1307 read error!  Preveri povezave."));
      Serial.println();
    } 
  }  
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(60);
  lcdA.clear();
  lcdA.print(F("Time:"));
  lcdA.setCursor(0, 1);
  if(timeStatus()!= timeSet) {
    sprintf(infoErr," Er T90 ");  
    Serial.println(infoErr);
    lcdA.print(infoErr);
  }  
  else
     lcdA.print(F(" RTC OK"));      

  delay(2000);
  NarediTimeStr(infoSet, now());
  lcdA.setCursor(0, 0);
  lcdA.print(infoSet);
  
  Serial.print(weekday());
  Serial.print(F("  "));
  
  Serial.print(day());
  Serial.print(F("."));
  Serial.print(month());
  Serial.print(F("."));
  Serial.print(year());
  Serial.print(F("  "));
  Serial.println(infoSet);
  

  delay(20);
  SDInit();  
  delay(200);
  EthernetInit(false);
  
  lcdA.clear();
  lcdA.print(F("Free RAM"));
  lcdA.setCursor(1, 1);
  lcdA.print(FreeRam());
  
  Serial.print(F("Free RAM: "));
  Serial.println(FreeRam()); 

  InitParametri();
  
  
//  NastaviTempCrpRad(limTempCrpRad, limTempFactCrpRad, crpRadAsAbsTemp);
  temeratureIzmerjene = true;
  prevCasMeritve = now()-30UL;
//  casMeritve = now();
  
  Serial.print(F("Stikala: "));
  PreveriStikala(true);
 
 /* 
  if (!manager.init()) {
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb5Fd10, power 0x10
  }
*/
  Serial.println(F(""));
  Encoder_init();
  Serial.println(F("Init. OK"));
  Beep(200);
  analogWrite(LCD_OSW_SW, 0);
    
}



//--------------------------------------------------------------------------------
void loop(void)
{ 
  
  static int prevDay = day();
  byte newCrpTCState;
//  static boolean hitEnd = true;
  
  
  //----- 
  if (cycStart > 0 && millis() > cycStart) {
//    if (hitEnd) {
//      hitEnd = false;
      unsigned long lastCycle = (millis() - cycStart);
//    sumCycle += (millis() - cycStart);
      sumCycle += lastCycle;
      ncyc ++;
      if (maxCycle == 0) {
        maxCycle = lastCycle;
        minCycle = lastCycle;
      }  
      else if (maxCycle < lastCycle) {
        maxCycle = lastCycle; 
      }   
      else if (minCycle > lastCycle) {
        minCycle = lastCycle;   
      }
//    }
  }  
  else {
    sumCycle = 0;
    ncyc = 0;
  }  
  cycStart = millis();  
  
  //----- 
  CheckSerial();
  CheckEthernet();
  Encoder_check();
//-----   
  
  stateCevStPecTC = digitalRead(CEVTERM_PEC_TC);

  //-----
  // če je od zadnje meritve preteklo več kot "merXMin*60"
  // preberi temperature
  // merXMin - število meritev na minuto
  // pošlju ukaz vsem senzorjem DS18x20 in preberi ostale senzorje

  if (temeratureIzmerjene) {
    if (now()/((unsigned long) merXMin*60) > prevCasMeritve/((unsigned long)merXMin*60)) {
      PreberiTemperature(!temeratureIzmerjene, true);
      temeratureIzmerjene = false;
      casMeritve = millis();
      Serial.print(F("'"));
      Serial.print(now() - prevCasMeritve);
    }
  }
  // po preteku convWaitTime preberi temperature DS18X20
  // convWaitTime - v ms
  if (temeratureIzmerjene == false) {
  if ((millis() >= (casMeritve + convWaitTime))) {

    Serial.print(F("/"));
    Serial.print(millis() - casMeritve);
    Serial.print(F("'"));
    PreberiTemperature(!temeratureIzmerjene, false);
    temeratureIzmerjene = true;
    
    if (izracHitrGret || izracHitrGretInfo) {
      IzracunHitrostiGretjaTC(); 
    }  

    if ((now() - lastTCStateChg > (1 + prevTCState*4.0)*(unsigned)minRunTimeMin * 60) && (now() - lastReleChg > (unsigned)minRunTimeMin * 60)) {   
      if (releState_TC == R_TC_OFF) {
        if (RefTemp() < TempVklopa()) {
          PreklopiTC(RELE_TC, R_TC_ON);
          if (seRracunaHitrGret) {
            izracHitrGret=true;
          }
          else {
            izracHitrGretInfo=true;  
          }
        }
      }  
      else if (RefTemp() > TempIzklopa())
        PreklopiTC(RELE_TC, R_TC_OFF);
    }
    ZapisiInIzpisiPodatke();
    prevCasMeritve = now();
//    SendToProc();
    CheckEthernet();
    DeviceHub();
  }
}
  delay(2);
  
  if (tok230V < 0)
    tok230V = AC_mimax();
  
  //----------------------
  // če je rele vklopljen
  // kontrola toka, če je kompresor vklopljen in start meritve porabe ...
  // stanje TC v -> prevTCState =  1
  if (releState_TC == R_TC_ON || measureOnly) {
    if (tok230V >= mejaToka) {
      if (AC_mimax() >= mejaToka) {
        if (prevTCState == 0) {
          prevTCState =  1;
          lastTCStateChg = now();
          startTemp = RefTemp();
          zacPorabaWH = porabaWH;
          tempOkolicaSt = cTemperatura[OKOLICA_0];
          u4.ulval = lastTCStateChg;
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
          delay(5);
          i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrPrevTCState, AT24C32_ADDR_LENGH, prevTCState);
          izracHitrGretInfo = true;
          ZapisiOnOffSD(1);
        }
      }
    }
  }
  if (prevTCState == 1) {
    if (tok230V < mejaToka) {
      if (AC_mimax() < mejaToka) {
        prevTCState = 0;
        lastRunTime = (now() - lastTCStateChg);
        onTimeTC += (now() - lastTCStateChg); 
        hitrGret = 3600.0 * (RefTemp() - startTemp)/((float) lastRunTime);
        u4.ulval = onTimeTC;
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4)); // write to EEPROM 
        ZapisiOnOffSD(0);
        lastTCStateChg = now();
        u4.ulval = lastTCStateChg;
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
        delay(5);
        i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrPrevTCState, AT24C32_ADDR_LENGH, prevTCState);
      }
    }
  }
  //--------------------
  // resetiranje in zapis podtkov konec dneva
  if (prevTCState == 0) {
    if (prevDay != day()) {
      onTimeTC = 0;
      u4.ulval = onTimeTC;
      i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4)); 
      sprintf(infoErr,"        ");
      porabaWH = 0;
        
      sumTok_12V = 0.0;
      nMerTok_12V = 0;
      maxTok_12V = 0;
        
      coRawValSum = 0.0;
      numMerCO = 0;
      coRawValMax = 0;
      
      onTimeCrpTC = 0;
      onTimeCrpRad = 0;
      
      prevDay = day();
    }
  }  

  StateCrpalkeRad();
  //!!!  lastCrpRadStateChg
  
  newCrpTCState = StatePovezTCPec(prevCrpTCState); 
  PovezTCPec(newCrpTCState);
  
  PreklopiVentilTCPec(100);
    
  if (now() > prevLCDizp) {
    prevLCDizp = now();
    IzpisiNaLCD();
  }
  Tok_12V();
  PreveriNapetosti(false, true, true, false);
  PreveriCO_Senzor();
//  hitEnd = true;
/*
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
//      if (!manager.sendtoWait(data, sizeof(data), from))
//        Serial.println("sendtoWait failed");
    
  }
  }
*/  
}

//--------------------------------------------------------------------------------
static byte StatePovezTCPec(byte state)
{

  if (preklopCrpTCVzr == 1) {
    if (IsNTempCas()) {
      preklopCrpTCVzr = 0; 
    }
  }
  
  if (preklopCrpTCVzr == 2) 
    if (prevCrpTCState == 0 &&  prevVentTCState == 0)
      if (stateCevStPecTC == CEV_TERM_OFF) 
        preklopCrpTCVzr = 0;
  
  if (preklopCrpTCVzr == 3) {
    if (prevTCState == 0) {
      preklopCrpTCVzr = 0;
    }
  } 
  
  if (manuCrpTCState > 0) {
    if (preklopCrpTCVzr != 0)
      preklopCrpTCVzr = 0;   
    return(state);
  }  
  
  if (prevCrpTCState == 0) {
    if (max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) > cTemperatura[CRPALKA_0] + minDiffTCPec) {
      if (max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) > tempVklopaCrpTC) {
        return(1);
      } 
      if (IsCasTransfTopl()) {
        return(1);
      }
      if (!IsNTempCas()) {
        if (RefTemp() < TempVklopaCrpTC_NTemp()) {
          preklopCrpTCVzr = 1;
          return(1);
        }
      }
      if (prevTCState == 1) {
        preklopCrpTCVzr = 3;
        return(1);
      }  
    }
    if ((stateCevStPecTC == CEV_TERM_ON) && (cTemperatura[CRPALKA_0] < cTemperatura[PEC_DV])) {
        preklopCrpTCVzr = 2;  // cevni termostat vklople
        return(1);
     }
     if (!IsCasTransfTopl()) {
       if (cTemperatura[PEC_DV] < (tempIzklopaVentCrpTC - 5.0)) {   
         if (cTemperatura[PEC_DV] > (tempIzklopaVentCrpTC - 12.5)) { 
           if (cTemperatura[CRPALKA_0] > tempIzklopaVentCrpTC) {
             if (pTempCrp[1] > tempIzklopaVentCrpTC && pTempCrp[2] > tempIzklopaVentCrpTC) {
               preklopCrpTCVzr = 4;
               return(1);  
             }
           }
         }    
       }
     }       
  }
  else {        // prevCrpTCState == 1
    if (preklopCrpTCVzr == 2) {
      if (stateCevStPecTC == CEV_TERM_OFF || (cTemperatura[CRPALKA_0] > cTemperatura[PEC_DV])) {
        return(0);
      }
    }
    else if (preklopCrpTCVzr == 4) {
      if ((cTemperatura[CRPALKA_0] <= tempIzklopaVentCrpTC) || 
          (min(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) > cTemperatura[CRPALKA_0]) || 
          (max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) < tempIzklopaVentCrpTC - 12.5))  {
        return(0);
      }
    } 
    else {  
      if ((cTemperatura[CRPALKA_0] >= max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV])) && 
            ((cTemperatura[CRPALKA_0] <= TempIzklopaCrpTC() || MaxCrpTCRunTime()) || IsNTempCas())) {
        return(0);
      }
      if (preklopCrpTCVzr > 0) {
        if (preklopCrpTCVzr == 1) {
          if (RefTemp() > TempIzklopaCrpTC_NTemp()) {
            return(0);  
          }
        }
        else if (preklopCrpTCVzr == 3) {
          if (prevTCState == 0) {
            return(0);
          }
        }         
      } else if (!IsCasTransfTopl()) {
        if ((max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) <= TempIzklopaCrpTC())  && cTemperatura[CRPALKA_0] <= TempIzklopaCrpTC()) {
          return(0);  
        }
      }  
    }
    //Kontrola vrenja 
    if (VodaVre(false)) {
      return(0);  
    }  
    //Kontrola vrenja  
  }
  return(state);  
}  


//--------------------------------------------------------------------------------  
void PovezTCPec(byte newState) {
  
  int zakasnitevVklopaSec;
  
  
  
  if (manuCrpTCState > 0) {
    if (manuCrpTCState == 3) {
      if (prevCrpTCState == 0) {
        if (prevVentTCState == 1) {
          zakasnitevVklopaSec = ZakasnitevVklopa(cTemperatura[PEC_TC_DV], tempVklopaCrpTC, 6); 
          if (RelaksTimeLimitSec(now(), lastVentTCChg[1], zakasnitevVklopaSec)) { 
            PreklopiCrpalkoTC(1);      // 1 - vklopi crpalko
          }
        }
        else {  //prevVentTCState == 0
          PreklopiVentilTCPec(1);
        }
      }
    }
    else if (manuCrpTCState == 1) {
      if (prevCrpTCState == 1) {
        PreklopiCrpalkoTC(0);
        manuCrpTCState = 0;
      }
      if (prevVentTCState == 1) {
        if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, zaksnitevCrpVent_Sec/2)) {
          PreklopiVentilTCPec(0);  
        }  
      }
    } 
    return;
  } 
  
  if (prevCrpTCState == 0) {
    if (newState == 1) {
      if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, minCrpTCRunTimeMin*60) && RelaksTimeLimitSec(now(), lastCrpRadStateChg, 1*60)) {
        if (prevVentTCState == 1) {
          zakasnitevVklopaSec = ZakasnitevVklopa(cTemperatura[PEC_TC_DV], tempVklopaCrpTC, 6);
          if (RelaksTimeLimitSec(now(), lastVentTCChg[1], zakasnitevVklopaSec)) { 
            PreklopiCrpalkoTC(1);      // 1 - vklopi crpalko
          }       
        }
        else if(RelaksTimeLimitSec(now(), lastVentTCChg[0], minCrpTCRunTimeMin*60)  || preklopCrpTCVzr == 2) {  //prevVentTCState == 0
          PreklopiVentilTCPec(1);
        }  
      }
    } // newState == 0
    else if (prevVentTCState == 1) {
      if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, zaksnitevCrpVent_Sec/2)) {
        if (IsCasTransfTopl()) {
          if (cTemperatura[CRPALKA_0] >= cTemperatura[PEC_DV]) {
            PreklopiVentilTCPec(0);
          }
        } else {              // !IsCasTransfTopl()
          if (preklopCrpTCVzr == 0) {
            if (cTemperatura[CRPALKA_0] < tempIzklopaVentCrpTC) {
              PreklopiVentilTCPec(0);
            }
          }
          else if (preklopCrpTCVzr == 1) {
            if (RefTemp() > TempIzklopaCrpTC_NTemp()) {
              PreklopiVentilTCPec(0);
            }
          }
          else if (preklopCrpTCVzr == 2) {
            if ((stateCevStPecTC == CEV_TERM_OFF) || ((cTemperatura[CRPALKA_0] > cTemperatura[PEC_DV]) && (cTemperatura[CRPALKA_0] < tempIzklopaVentCrpTC))) {
              PreklopiVentilTCPec(0);
            }
          }  
        }
      }
    }
    if (prevVentTCState == 0) {
      if (!IsCasTransfTopl() && !IsNTempCas()) {
        if (cTemperatura[CRPALKA_0] > tempIzklopaVentCrpTC) {
          if (max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) > minTempPecPonovnoOdpVent) {
            if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, zaksnitevCrpVent_Sec/2)) {
              PreklopiVentilTCPec(1);  
            }
          }  
        }  
      }  
    }    
  }    // prevCrpTCState == 1
  else  {      
    if (newState == 0) {
      if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, minCrpTCRunTimeMin*60)  || preklopCrpTCVzr == 2) {
        PreklopiCrpalkoTC(0);      // 0 - izklopi crpalko
        if (preklopCrpTCVzr != 0) {
          preklopCrpTCVzr = 0; 
        }
      }
    }
  }
  
 
  
  if (prevVentTCState == 255) {
    if (cTemperatura[CRPALKA_0] + minDiffTCPec > tempIzklopaVentCrpTC || prevCrpTCState == 1) {
       PreklopiVentilTCPec(1);         //1 - odpre ventil
    } else {
       PreklopiVentilTCPec(0);         //1 - odpre ventil
    }   
  }
//  PreklopiVentilTCPec(100);  
  
}











//--------------------------------------------------------------------------------
void StateCrpalkeRad()
{

  if (prevCrpRadState == 0) {
    if (cTemperatura[PEC_DV] - cTemperatura[OKOLICA_0] > minDiffDvOkolCrpRad) {
      if (cTemperatura[RAD_PV] < MejnaTempPreklCrpRad(1)) {
        if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
          PreklopiCrpalkoRad(1);
        }
      }
    }
    if (cTemperatura[RAD_PV] > (cTemperatura[RAD_DV] + 2.0*dTemp)) {
      if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
          PreklopiCrpalkoRad(1);
      }  
    }  
  }
  else {
    if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
      if (cTemperatura[RAD_PV] >= MejnaTempPreklCrpRad(0)) { 
        PreklopiCrpalkoRad(0);  
      }
      else if (cTemperatura[PEC_DV] - cTemperatura[OKOLICA_0] < minDiffDvOkolCrpRad) {
        PreklopiCrpalkoRad(0);
      }
    }  
  }  
}







//--------------------------------------------------------------------------------
//
static void  PreklopiTC(int relePin, int newState)
{
 
/*  
  #ifdef TC_EL_GRELEC
   
  #endif
  if (releTC = )
*/  
  if (measureOnly)
    digitalWrite(relePin, R_TC_ON);
  else  
    digitalWrite(relePin, newState);
  
  releState_TC = newState;
  lastReleChg = now();  
}

//--------------------------------------------------------------------------------
//
/*
uint8_t * heapptr, * stackptr;
void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}
*/




//--------------------------------------------------------------------------------









//--------------------------------------------------------------------------------









//--------------------------------------------------------------------------------------------
/*
float Sec2Hour(float sec) {

  return(sec / 3600.0);
}         
*/



