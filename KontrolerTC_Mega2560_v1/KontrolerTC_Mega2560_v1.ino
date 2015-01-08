



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

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>


#include "Configuration.h"
#include "Eprom_external.h"


#include "Kontroler_TC.h"

#include "Tok_napetost.h"
#include "Temperature.h"

#include "LCD_funkcije.h"
#include "Encoder_butt.h"
#include "Zapisi_sd.h"
#include "Ethernet_funk.h"
#include "Gas_Sensor.h"

static void PreklopiVentil(byte newState);
// splosne spremenljivke



//unsigned long casMeritve;

int prevMinute = -1;


//unsigned long startProg;








// rele spremenljivke
//boolean releState_1 = R_TC_OFF;  // stanje releja vklopa TC
unsigned long lastReleChg;    // cas zadnje spremembe stanja releja TC

// LCD


//extern int modeLCD;                // slika izpisanega zaslona 
 unsigned long prevLCDizp;     // cas zadnjega izpisa na LCD

// splosne spremenljivke





unsigned long lastCrpTCStateChg;
unsigned long lastVentTCChg[2];

int preklopCrpTCVzr = 0;

byte prevCrpTCState = 0;
byte prevVentTCState = 255;

byte manuCrpTCState = 0;

int stateCevStPecTC;


boolean isCrpRadAsAbsTemp = true; //upobi absol. temp za zagon crpalke
//float crpRadAsAbsTemp[24];





//unsigned long lastPavza;


float pTempCrp[3];


boolean izracHitrGret = false;
boolean izracHitrGretInfo=false;
boolean seRracunaHitrGret;

//--------------------------------------------------------------------------------
// main function to print information about a device
static void PrintData()
{
  char cas[13];
 
  NarediTimeStr(cas, now());
  Serial.print(cas);

  Serial.print(F(" -> "));
  PrintTemperatureAll();
  
  if (releState_1 == R_TC_ON) {
    if (prevTCState == 1) 
       Serial.print(F("ON"));
     else
       Serial.print(F("SB"));
   }
   else
     Serial.print(F("OFF"));

}



#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
//     #define RH_RF24_MAX_MESSAGE_LEN 28
    
  RH_NRF24 driver(NRF24_CE, NRF24_CSN);
  RHReliableDatagram manager(driver, CLIENT_ADDRESS);

//  #define RH_RF24_MAX_MESSAGE_LEN
//--------------------------------------------------------------------------------
void setup(void)
{
  
  char infoSet[9];
//  boolean exist = true;
//  DeviceAddress tmpAddr;
  
  NastavitevPinov();

  Serial.begin(115200);
  Serial.println(VERSION);
  
  
    //-------------------
  Wire.begin();
  
  //-------------------
  LCDInitializacija();
  EthernetInit();
  
  PreveriNapetosti(true, true, false);
  Serial.println(F(""));
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
  

  delay(2000);
   
  SDInit();  

  delay(2000);
  
  lcdA.clear();
  lcdA.print(F("Free RAM"));
  lcdA.setCursor(1, 1);
  lcdA.print(FreeRam());
  
  Serial.print(F("Free RAM: "));
  Serial.println(FreeRam()); 

  InitParametri();
  
//  prevCasMeritve = 0;

  
  
//  NastaviTempCrpRad(limTempCrpRad, limTempFactCrpRad, crpRadAsAbsTemp);
  temeratureIzmerjene = true;
  prevCasMeritve = now();
  casMeritve = now();
  
  Serial.print(F("Stikalo crpalke rad: "));
  if (digitalRead(STIKALO_CRP_RAD_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_CRP_RAD_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));
  else
    Serial.println(F("AUTO")); 
    
  Serial.print(F("Stikalo crpalke TC: "));
  if (digitalRead(STIKALO_CRP_TC_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_CRP_TC_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));  
  else
    Serial.println(F("AUTO"));
  
  Serial.print(F("Stikalo 3: "));
  if (digitalRead(STIKALO_TC_ON) == STIKALO_ON)
    Serial.println(F("ON"));
  else if (digitalRead(STIKALO_TC_OFF) == STIKALO_ON)
    Serial.println(F("OFF"));  
  else
    Serial.println(F("AUTO"));
  
  
  if (!manager.init()) {
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb5Fd10, power 0x10
  }

  Serial.println(F(""));
  Encoder_init();
  Serial.println(F("Init. OK"));
  Beep(200);
  analogWrite(LCD_OSW_SW, 0);
//  while (now()/(merXMin*60) == prevCasMeritve/(merXMin*60)) 
//    delay(100);
    
}

//--------------------------------------------------------------------------------
unsigned long cycStart;
unsigned long sumCycle;
unsigned long ncyc;

//--------------------------------------------------------------------------------
static float AvgCycleTime(unsigned long sum, unsigned long num)
{
  if (num > 0)
    return((float)sum/(float)num);
  return(-1.0);
}





//--------------------------------------------------------------------------------
void loop(void)
{ 
  float tok = -100.0;
  char casun[5];
  byte newCrpTCState;
  float alpha;
  char c;
  
  
   
  if (cycStart > 0 && millis() > cycStart) {
    sumCycle += (millis() - cycStart);
    ncyc ++;
  }
  else {
    sumCycle = 0;
    ncyc = 0;
  }  
  cycStart = millis();  
   
     if (Serial.available() >  0 ) {
       c = Serial.peek();
     }  
     if (Serial.available() >=  TIME_MSG_LEN ) {
       casun[0]= c;  
       if (casun[0] ==  TIME_HEADER) {
         time_t t = processSyncMessage();
         if(t >0)  {
           RTC.set(t);   // set the RTC and the system time to the received value
           setTime(t);
           Serial.println(F("Cas je nastavljen"));
           prevCasMeritve = now();        
         }
       }   
     }  
     
     else if (Serial.available() == 3) {
//       c = Serial.peek();
       Serial.println(F(""));
       Serial.print(c);
       
       if (c == 'p') {
         c = Serial.read();
         c = Serial.read();
         Serial.print(c);
         if (c == 'o')
           IzpisDataOnOffSerial();  
         if (c == 'a')
           IzpisDatnaSerial();
         Serial.read();
       
       }
       else if (c == 'm') {
         c = Serial.read();
         c = Serial.read();
         Serial.print(c);
         if (c == 'c') {
           c = Serial.read();
           Serial.print(c);
           if (c == '0') {
             manuCrpTCState = B01;
             
           }
           else if (c == '1') {
             manuCrpTCState = B11;
           }
           else {
             manuCrpTCState = 0;
           }
           Serial.print(F("  "));
           Serial.print(manuCrpTCState);  
         }
         
         
         if (c == 'r') {      //mrX
           c = Serial.read();
           Serial.print(c);
           if (c == '0') {
             PreklopiCrpalkoRad(0);;
             
           }
           else if (c == '1') {
             PreklopiCrpalkoRad(1);;
           }
         }       
       }
       while (Serial.available() > 0) {
         Serial.print(Serial.read());  
       }  
       Serial.flush();  
     }
   else {
     while (Serial.available() > 0) {
         Serial.print(Serial.read());  
       } 
     Serial.flush();  
   }  
//     else 
//       delay(10);
//  } while (millis() - lastPavza < 250);
//  lastPavza = millis();
  
  
  // wait for an incoming TCP packet, but ignore its contents
 
/*
  if (ether.packetLoop(ether.packetReceive())) {
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }
*/  
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
 
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data

  Encoder_check();
  
  stateCevStPecTC = digitalRead(CEVTERM_PEC_TC);
  
  if (((now()/(merXMin*60)) > (prevCasMeritve/(merXMin*60))) && temeratureIzmerjene == true) {  /* || (minute() < 30 && prevMinute > 30)*/
//    temeratureIzmerjene = PreberiTemperature(false, true);
    PreberiTemperature(false, true);
    temeratureIzmerjene = false;
    casMeritve = now();
    
  }
  if ((now() >= (casMeritve + convWaitTime/1000)) && (temeratureIzmerjene == false)) {
//  if (now()/(merXMin*60) > prevCasMeritve/(merXMin*60)) {  /* || (minute() < 30 && prevMinute > 30)*/
//    temeratureIzmerjene = PreberiTemperature(true, false);
    PreberiTemperature(true, false);
    temeratureIzmerjene = true;
    casMeritve = now();

    
 //   if ((now() - lastTCStateChg > (1 + prevTCState*0.5)*minRunTimeMin * 60) && (now() - lastReleChg > minRunTimeMin * 60)) {
   
   if (izracHitrGret || izracHitrGretInfo) {
      if (prevTCState == 0) {
        if (now() - lastTCStateChg > 15*60L) {
          
          alpha = 0.99;
          if (izracHitrGret) {  
            alpha = 0.9;
          }
          else {
            alpha = 0.99;
          }
          
          lastDeltaTh = IzracDeltaTh();     
          deltaTh = PovpreciVred(alpha, deltaTh, lastDeltaTh);
          
          lastDeltaThOk = IzracDeltaThOk();
          deltaThOk = PovpreciVred(alpha, deltaThOk, lastDeltaThOk);
          
          lastDeltaThSt = IzracDeltaThSt();
          deltaThSt = PovpreciVred(alpha, deltaThSt, lastDeltaThSt);
          
          
//          deltaTh = (deltaTh * alpha + (1.0-alpha) * lastDeltaTh);
          
//         deltaThOk = (deltaThOk * alpha + (1.0-alpha) * lastDeltaThOk);
          
           
          Serial.println(F(""));
          if (izracHitrGretInfo) {
            Serial.println(F("Info - "));
          }
          
          Serial.print(F(" H.gret:"));
          Serial.print(deltaTh, 3);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaTh, 3);
          uf.fval =  deltaTh;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          Serial.print(F(" Komp.ok:"));
          Serial.print(deltaThOk, 4);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaThOk, 4);
          uf.fval =  deltaThOk;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          Serial.print(F(" Komp.st:"));
          Serial.print(deltaThSt, 4);
          Serial.print(F(" Last:"));
          Serial.print(lastDeltaThSt, 4);
          uf.fval =  deltaThSt;
          delay(5);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
          
          
          Serial.print(F(" Info cal.(startTemp): "));
          Serial.print(startTemp, 2);
          Serial.print(F(" temp okolice st: "));
          Serial.print(tempOkolicaSt, 2);
          Serial.print(F(" run time(h): "));
          Serial.print(Sec2Hour(lastRunTime), 4);
          Serial.print(F(" tkomp0: "));
          Serial.print(tKompOK, 2);
          Serial.print(F(" tkompSt: "));
          Serial.print(tKompSt, 2);
          
          
          Serial.print(F(" Nova komp.ok: "));
          Serial.print(KompenzacijaTempOkolice(tempOkolicaSt),4);
          Serial.print(F(" komp.st: "));
          Serial.print(KompenzZacTemp(startTemp),4);
          
          Serial.print(F(" Info deltaTh: "));
          Serial.print(IzracDeltaTh(),4);
          Serial.print(F(" Info deltaThOk: "));
          Serial.print(IzracDeltaThOk(),4);
          Serial.print(F(" Info deltaThSt: "));
          Serial.print(IzracDeltaThSt(),4);
          
          Serial.print(F(" Cop: "));
          Serial.print(Cop(),2);
          
          izracHitrGret=false;
          izracHitrGretInfo=false;
          if (izracHitrGret) {
            ZapisiOnOffSD(10, 100);
          }  
          else {
            ZapisiOnOffSD(11, 100);
          }        
        }
      }  
    }  

   if ((now() - lastTCStateChg > (1 + prevTCState*4.0)*(unsigned)minRunTimeMin * 60) && (now() - lastReleChg > (unsigned)minRunTimeMin * 60)) {   
      if (releState_1 == R_TC_OFF) {
        if (RefTemp() < TempVklopa()) {
          PreklopiRele(RELE_TC, R_TC_ON);
          if (seRracunaHitrGret)
            izracHitrGret=true;
          else
            izracHitrGretInfo=true;  
        }
      }  
      else if (RefTemp() > TempIzklopa())
        PreklopiRele(RELE_TC, R_TC_OFF);
    }

    
    
    Serial.println(F(""));   
    PrintData();
    
 //   printDataRF();
    
    
    
    
    
    
    
    
    Serial.print(F(" "));
    Serial.print(prevCrpTCState);
    Serial.print(prevVentTCState);
    Serial.print(prevCrpRadState);
    Serial.print(stateCevStPecTC);
    Serial.print(F(" "));
    Serial.print(manuCrpTCState, BIN);
    Serial.print(F(" "));
    Serial.print(preklopCrpTCVzr);

    
    if (UpostevajElTarife())
      Serial.print(F(" E"));
    else 
      Serial.print(F(" P"));  
    
    
    if (prevCasMeritve/(zapisXMin*60) < now()/(zapisXMin*60)) {     
      Serial.println(F(""));
//      for (int j = 0; j < numSensDS; j++) {
      for (int j = 0; j < numSens; j++) {
//        addrTmp = elapsedSecsToday(now())/(zapisXMin*60);
        addrTmp = ObsegZgodovine(j);
//        addrTmp += (j * histLen);    
//        addrTmp *= sizeof(u2);
//        addrTmp += addrTempBack;
 //     addrTmp = addrTempBack + sizeof(uf)*((elapsedSecsToday(now())/(zapisXMin*60)) + (day()-1)*144); //1440/zapisXMin
        delay(10);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        delay(10);

        Serial.print(F(" Ta"));
        Serial.print(j+1);
        Serial.print(F(":"));
        
        Serial.print(F("("));
        Serial.print(u2.uival/100.0);
        
        Serial.print(F(">"));
        Serial.print(addrTmp);
        
        Serial.print(F(")"));
        
        // dodano kot kontrola vrenja
        if (j == CRPALKA_0) {
          pTempCrp[2] = pTempCrp[1];
          pTempCrp[1] = pTempCrp[0];
          pTempCrp[0] = cTemperatura[j];
          if (VodaVre(false))
            Serial.print(F("P "));
          else
            Serial.print(F("V "));  
        }
        // dodano kot kontrola vrenja  
        
        float diff = cTemperatura[j] - (u2.uival/100.0);
        if (abs(diff) > 0.01) {
          sumTemp[j] -= (u2.uival)/100.0;
          u2.uival =  (cTemperatura[j]+0.005)*100;
          sumTemp[j] += (cTemperatura[j]);

          delay(10);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        }

        Serial.print(AvgVal(sumTemp[j], histLen*1.0),2);

      }
      Serial.println(F(""));

      

/*      
      last24H_Info();
      Serial.print(F("24h"));
      for (int i=0; i<numSensDS; i++) {
        Serial.print(F("  T"));
        Serial.print(i+1);
        Serial.print(F(" min:"));
        Serial.print(minTemp24[i]);
        Serial.print(F("@"));
        Serial.print(day(minTemp24Time[i]));
        Serial.print(F(" "));
        Serial.print(hour(minTemp24Time[i]));
        Serial.print(F(":"));
        if (minute(minTemp24Time[i]) < 10)
          Serial.print(F("0"));  
        Serial.print(minute(minTemp24Time[i]));
        Serial.print(F(": max:"));
        Serial.print(maxTemp24[i]);
        Serial.print(F("@"));
        Serial.print(day(maxTemp24Time[i]));
        Serial.print(F(" "));
        Serial.print(hour(maxTemp24Time[i]));
        Serial.print(F(":"));
        if (minute(maxTemp24Time[i]) < 10)
          Serial.print(F("0")); 
        Serial.print(minute(maxTemp24Time[i]));
        Serial.print(F(" avg:"));
        Serial.print(sum24[i]/24.0, 2);
        if (i==2 || i==5)
          Serial.println(F(""));  
      }
*/      
      Serial.print(F("  Avg.cycleTime(n="));
      Serial.print(ncyc);
      Serial.print(F("): "));
      Serial.print(AvgCycleTime(sumCycle, ncyc));
      Serial.println(F("ms"));    

      PreveriNapetosti(true, true, false);
      Serial.println(F(""));
    }
    PrintTempAllSDbin();  
    
    
    
    
    Serial.print(F(" Tok: "));
    tok = AC_mimax(showCRC, true);
    Serial.print(tok);  
    Serial.print(F("A("));
    
    porabaWH += (tok *(230.0/60.0));
    if (porabaWH >= 1000) {
       if (porabaWH/1000.0 > 100)
         Serial.print(porabaWH/1000.0, 1);
       else if (porabaWH/1000.0 > 10)
         Serial.print(porabaWH/1000.0, 2);
       else 
         Serial.print(porabaWH/1000.0, 3);  
       Serial.print(F("kVAh"));
    }
    else {
      if (porabaWH > 100)
        Serial.print(porabaWH, 1);
      else if (porabaWH > 10)
        Serial.print(porabaWH, 2);
      else
        Serial.print(porabaWH, 3);  
      Serial.print(F("VAh"));  
    }
    Serial.print(F(") "));
    
    Serial.flush();
    Serial.print(F("On time: "));
     

    if (prevTCState == 0) {
      if (onTimeTC > 0) 
      Serial.print(Sec2Hour(onTimeTC));  
    }
    else if (prevTCState == 1)
       Serial.print(Sec2Hour(onTimeTC + now() - lastTCStateChg));


     
     Serial.print(F(" Tv:"));
     Serial.print(TempVklopa(), 3);  
     Serial.print(F(" Ti "));
     Serial.print(TempIzklopa(), 1);
     Serial.print(F(" ("));
     Serial.print(TempVklopaCrpTC_NTemp(), 1);
     Serial.print(F("-"));
     Serial.print(TempIzklopaCrpTC_NTemp(), 1);
     Serial.print(F("->"));
     Serial.print(MejnaTempPreklCrpRad(0), 1);
     Serial.print(F("-"));
     Serial.print(MejnaTempPreklCrpRad(1), 1);
//     Serial.print(F(") "));
     
     Serial.print(F(") CO:"));
 #ifdef CO_INIT    
     Serial.print((float)coRawVal/(float)numMerCO, 2);
     numMerCO = 0;
     coRawVal = 0;
 #else
     Serial.print(coRawVal);
 #endif 
     Serial.print(F(" "));
     
     Serial.print(osvetlitevLCD);
      
/*     Serial.print(F("Stikalo crpalke rad: "));
     Serial.print(F("  "));
     int info1 = digitalRead(STIKALO_CRP_RAD_ON);
     Serial.print(info1);
     delay(2);
     info1 = digitalRead(STIKALO_CRP_RAD_OFF);
     Serial.print(info1);
     delay(2);
     info1 = digitalRead(STIKALO_CRP_TC_ON);
     Serial.print(info1);
     delay(2);
     info1 = digitalRead(STIKALO_CRP_TC_OFF);
     Serial.print(info1);
     info1 = digitalRead(STIKALO_TC_ON);
     Serial.print(info1);
     delay(2);
     info1 = digitalRead(STIKALO_TC_OFF);
     Serial.print(info1);
*/     
     if (digitalRead(STIKALO_CRP_RAD_ON) == STIKALO_ON)
       Serial.print(F(" ON"));
     else if (digitalRead(STIKALO_CRP_RAD_OFF) == STIKALO_ON)
       Serial.print(F(" OFF"));
     else
       Serial.print(F(" AUT")); 
    
//     Serial.print(F("Stikalo crpalke TC: "));
     if (digitalRead(STIKALO_CRP_TC_ON) == STIKALO_ON)
       Serial.print(F(" ON"));
     else if (digitalRead(STIKALO_CRP_TC_OFF) == STIKALO_ON)
       Serial.print(F(" OFF"));  
     else
       Serial.print(F(" AUT"));
    
     if (digitalRead(STIKALO_TC_ON) == STIKALO_ON)
       Serial.print(F(" ON"));
     else if (digitalRead(STIKALO_TC_OFF) == STIKALO_ON)
       Serial.print(F(" OFF"));  
     else
       Serial.print(F(" AUT"));

    prevCasMeritve = now();
  }
  delay(2);
  
  if (tok < 0)
    tok = AC_mimax();
  
  if (releState_1 == R_TC_ON || measureOnly) {
    if (tok >= mejaToka) {
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
    if (tok < mejaToka) {
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
  if (prevTCState == 0) {
    if (onTimeTC > 0) {
      if (day(lastTCStateChg) != day()) {
        onTimeTC = 0;
        u4.ulval = onTimeTC;
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4)); 
        sprintf(infoErr,"        ");
        porabaWH = 0;
      }
    }
  }  

  StateCrpalkeRad();
  
  newCrpTCState = StatePovezTCPec(prevCrpTCState); 
  PovezTCPec(newCrpTCState);
  
  PreklopiVentil(100);
    
  if (now() > prevLCDizp) {
    prevLCDizp = now();
    IzpisiNaLCD();
  }
  PreveriCO_Senzor();
  
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
/*
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
 */       Serial.println("sendtoWait failed");
    
  }
  }
  
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
//       if (min(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) < tempIzklopaVentCrpTC - 5.0) {   
         if (cTemperatura[PEC_DV] > (tempIzklopaVentCrpTC - 12.5)) {
//         if (max(cTemperatura[PEC_DV], cTemperatura[PEC_TC_DV]) > tempIzklopaVentCrpTC - 12.5) {  
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
static boolean VodaVre(boolean izpis) {
  float dTemp;
  
//  dTemp = (float) abs((float) pTempCrp[2] - (float) cTemperatura[CRPALKA_0]);
//  dTemp += ((float) abs((float) pTempCrp[1] - (float) cTemperatura[CRPALKA_0]));
  
  if (cTemperatura[PEC_TC_DV] < tempVklopaCrpTC) {
    return(false); 
  } 
  
  dTemp = sq(pTempCrp[2] - cTemperatura[CRPALKA_0]);
  dTemp += (sq(pTempCrp[1] - cTemperatura[CRPALKA_0]));
  dTemp = sqrt(dTemp);
  
  if (izpis) {
    Serial.print(F(" <DTemp: "));
    Serial.print(dTemp);
    Serial.print(F(" "));
    Serial.print(pTempCrp[2]);
    Serial.print(F(" "));
    Serial.print(pTempCrp[1]);
    Serial.print(F("> "));
  }
  
  if (dTemp < 0.1)
    return(true);
  return(false);  
}  


//--------------------------------------------------------------------------------
static boolean MaxCrpTCRunTime()
{
  return(RelaksTimeLimitSec(now(), lastCrpTCStateChg, 30*60)); // 30 min  
}

//--------------------------------------------------------------------------------
float TempVklopaCrpTC_NTemp() {
  float tmpTemp;
  
  if (!IsWeekend()) {
    tmpTemp = max(TempVklopa() + minDiffTCPec, (TempVklopa() +  ciljnaTemp)/2.0);  
    return(tmpTemp);
  }
  return(ciljnaTemp - minDiffTCPec);
}

//--------------------------------------------------------------------------------
static float TempIzklopaCrpTC_NTemp() {
  float tmpTemp;
  
  if (!IsWeekend()) {
    tmpTemp = max(TempIzklopa() + minDiffTCPec, histCrpTC + (TempVklopa() +  ciljnaTemp)/2.0);  
    return(tmpTemp);
  }
  return(ciljnaTemp + minDiffTCPec);
}
//--------------------------------------------------------------------------------  
static void PovezTCPec(byte newState) {
  
  int zakasnitevVklopa;
  
  if (manuCrpTCState > 0) {
    if (manuCrpTCState == 3) {
      if (prevCrpTCState == 0) {
        if (prevVentTCState == 1) {
          if (cTemperatura[PEC_TC_DV] > tempVklopaCrpTC) {
            zakasnitevVklopa = 6 * zaksnitevCrpVent_Sec;
          }
          else {
            zakasnitevVklopa = zaksnitevCrpVent_Sec;
          }  
          if (RelaksTimeLimitSec(now(), lastVentTCChg[1], zakasnitevVklopa)) { 
            PreklopiCrpalkoTC(1);      // 1 - vklopi crpalko
          }
        }
        else {  //prevVentTCState == 0
          PreklopiVentil(1);
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
          PreklopiVentil(0);  
        }  
      }
    } 
    return;
  } 
  
  if (prevCrpTCState == 0) {
    if (newState == 1) {
      if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, minCrpTCRunTimeMin*60) && RelaksTimeLimitSec(now(), lastCrpRadStateChg, 1*60)) {
        if (prevVentTCState == 1) {
          if (cTemperatura[PEC_TC_DV] > tempVklopaCrpTC) {
            zakasnitevVklopa = 6 * zaksnitevCrpVent_Sec;
          }
          else {
            zakasnitevVklopa = zaksnitevCrpVent_Sec;
          }   
          if (RelaksTimeLimitSec(now(), lastVentTCChg[1], zakasnitevVklopa)) { 
            PreklopiCrpalkoTC(1);      // 1 - vklopi crpalko
          }
        }
        else if(RelaksTimeLimitSec(now(), lastVentTCChg[0], minCrpTCRunTimeMin*60)  || preklopCrpTCVzr == 2) {  //prevVentTCState == 0
          PreklopiVentil(1);
        }  
      }
    } // newState == 0
    else if (prevVentTCState == 1) {
      if (RelaksTimeLimitSec(now(), lastCrpTCStateChg, zaksnitevCrpVent_Sec/2)) {
        if (IsCasTransfTopl()) {
          if (cTemperatura[CRPALKA_0] >= cTemperatura[PEC_DV]) {
            PreklopiVentil(0);
          }
        } else {              // !IsCasTransfTopl()
          if (preklopCrpTCVzr == 0) {
            if (cTemperatura[CRPALKA_0] < tempIzklopaVentCrpTC) {
              PreklopiVentil(0);
            }
          }
          else if (preklopCrpTCVzr == 1) {
            if (RefTemp() > TempIzklopaCrpTC_NTemp()) {
              PreklopiVentil(0);
            }
          }
          else if (preklopCrpTCVzr == 2) {
            if ((stateCevStPecTC == CEV_TERM_OFF) || (cTemperatura[CRPALKA_0] > cTemperatura[PEC_DV])) {
              PreklopiVentil(0);
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
              PreklopiVentil(1);  
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
       PreklopiVentil(1);         //1 - odpre ventil
    } else {
       PreklopiVentil(0);         //1 - odpre ventil
    }   
  }
//  PreklopiVentil(100);   
}

//-------------------------------------------------------------------------------- 
static boolean RelaksTimeLimitSec(unsigned long cTime, unsigned long pTime,int rTime)
{
  if (cTime - pTime > (unsigned long)rTime)
    return(true);
  return(false);  
}

//--------------------------------------------------------------------------------
void PreklopiCrpalkoTC(byte newState)
{
  char cas[13];
  
//  if (newState < 255) {
    
    if (newState == 1) {
      if (preklopCrpTCVzr == 0) {
        if (cTemperatura[PEC_TC_DV] >= tempVklopaCrpTC || cTemperatura[PEC_DV] >= tempVklopaCrpTC) {
          if ((cTemperatura[PEC_PV] < tempVklopaCrpTC - 3.0*dTemp) && (cTemperatura[RAD_PV] < tempVklopaCrpTC - 3.0*dTemp)) {
            return;
          }  
        }
      }  
    }
    
    if (newState == 1) {
      digitalWrite(RELE_CTC, R_CTC_ON);
      
      ZapisiOnOffSD(1, 2);
      
      Serial.println(F(" "));
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(F(" Vklop crpalke"));
      
    }
    else {
      digitalWrite(RELE_CTC, R_CTC_OFF);
      
      ZapisiOnOffSD(0, 2);
      
      Serial.println(F(" "));
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(F(" Izklop crpalke"));
      
      ResetCrpTCVzr();
    }
    prevCrpTCState = newState;
    lastCrpTCStateChg = now();
//  }
}

//--------------------------------------------------------------------------------
static void ResetCrpTCVzr()
{
  preklopCrpTCVzr = 0; 
}
//--------------------------------------------------------------------------------
static void PreklopiVentil(byte newState)
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
      
      ZapisiOnOffSD(1, 1);
      
      Serial.println(F(" "));
      NarediTimeStr(cas, now());
      Serial.print(cas);
      Serial.print(" ");
      Serial.print(millis());
      Serial.print(F(" Odpiram ventil"));
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
      Serial.print(F(" Zapiram ventil"));
    }
  }
}


//--------------------------------------------------------------------------------
float TempIzklopaCrpTC() 
{
  return(tempVklopaCrpTC - histCrpTC);
}  

//--------------------------------------------------------------------------------
static void StateCrpalkeRad()
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
static float MejnaTempPreklCrpRad(byte newState)
{
  // dodaj kot glob
  int ref_prostor_1 = 0;
  float kTemp = 0.25;
  float cTemperaturaZun = 0.0;
  // mogce spremeni abs temp
  float zeljTemp;
  
  
  float refTemp;
  float tmpTemp;
  float dTemp = 0.0;
  
  refTemp = (float)limTempCrpRad[hour()] + limTempFactCrpRad[hour()] * cTemperatura[RAD_DV];
  if (newState == 1) {
    tmpTemp = cTemperatura[RAD_DV] - refTemp;
    if (isCrpRadAsAbsTemp) {
      zeljTemp =  (float)crpRadAsAbsTemp[hour()] - 10.0;
 
        dTemp = (zeljTemp - cTemperaturaZun) * kTemp; 
 //       if (cTemperatura[ref_prostor_1] < zeljTemp) {
        dTemp *= (zeljTemp / cTemperatura[ref_prostor_1]);
 //     }
      tmpTemp = min((float)crpRadAsAbsTemp[hour()] + dTemp, tmpTemp);
    }
    if (cTemperatura[PEC_DV] >  maxTempDVPec) {
      return(maxTempDVPec);  
    }
    if (tmpTemp < 5.00)  {
      return(5.0);
    }  
    return(tmpTemp);
  }
  else {
    refTemp *= minMejnaTempRel;
    if (refTemp < 5.0)
      refTemp = 5.0;
    if (cTemperatura[RAD_DV] - refTemp > maxTempPVRad)
      return(maxTempPVRad);
    
    if (cTemperatura[RAD_DV] - refTemp < 5.00)  {
      return(5.0);
    }  
    return(cTemperatura[RAD_DV] - refTemp);
  }
}

//--------------------------------------------------------------------------------
static void PreklopiCrpalkoRad(byte newState)
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
//



//--------------------------------------------------------------------------------
//
static void  PreklopiRele(int relePin, int newState)
{
  
  releState_1 = newState;
  if (measureOnly)
    digitalWrite(relePin, R_TC_ON);
  else  
    digitalWrite(relePin, newState);
  
  lastReleChg = now();  
}

//--------------------------------------------------------------------------------
//

static float IzracunLimitTemp(int state, float ciTemp)
{
  float tUra;
  float refTemp;
  
  if (hour() < uraVTemp[0])
    tUra = (float)uraVTemp[0] - (float) hour()-1.0;  
  else 
    tUra = abs((float) hour()-24.0) + (float) uraVTemp[0] - 1.0;
  
  
//refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) / KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]);  
  
//  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0)));
//  refTemp *= KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]);
//  refTemp *= KompenzZacTemp(cTemperatura[CRPALKA_0]);
  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) * KompenzacijaTempOkolice(cTemperatura[OKOLICA_0]) * KompenzZacTemp(cTemperatura[CRPALKA_0]);  


  
  if (state == 0) {  
//    return(max(minTempNightOn, refTemp + sensDiff/*+dTemp*/)); // !!!
    return(max(minTempNightOn, refTemp + deltaTh/2.0/*+dTemp*/)); // !!!
  }
  else if (state == 1) {
    if (hour() >= uraVTemp[0] - 2 && hour() < uraVTemp[1])
      return(ciTemp+(dTemp*2.0));
    else {  
//      refTemp = max(minTempNightOn+dTemp, refTemp+dTemp*2.0);
      refTemp = max(minTempNightOn+dTemp, refTemp+dTemp+deltaTh);
      return(min(ciTemp+(dTemp*2.0), refTemp));
    }
  }
  else 
    return(refTemp);
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
static float IzracunTempVTOff(void)
{
  float rel;
  float relT;
  
  relT = (float) uraVTemp[1] - dolPrehObd/60.0;
  
  if ((float) hour() < relT)
    return(minTempVTOn + dTemp);
  
  rel = (float) hour() + (minute()/60.0) - relT;
  rel = rel/((float) uraVTemp[1] - relT);
  relT = minTempVTOn + dTemp*(1.0-rel);
  return(relT);
}


/*
static int FreeMemory() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
*/

uint8_t * heapptr, * stackptr;
void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}


//--------------------------------------------------------------------------------
static void IzpisDatnaSerial(void)
{
 // char ch;
  char ime[12];
  
  ImeDatoteke(ime);
  Serial.println(F(""));
  Serial.println(ime);
  
  myFile = SD.open(ime, FILE_READ);
  if (!myFile)
      Serial.println("error"); 
  else {
    while (myFile.available()) {
      Serial.write(myFile.read());
      delay(5); 
    }
    myFile.close();
  } 
//  myFile.close(); 
  Serial.println(F(""));  
  delay(5);  
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
float TempVklopa(void)
{
  seRracunaHitrGret = false;
  
  if (IsWeekend()) {
    if (IsNTempCas()) {
        if (weekday() == NED && hour() > uraVTemp[0]) {
          seRracunaHitrGret = true;
          return(IzracunLimitTemp(0, ciljnaTemp));  
        }  
      return(minTempNightOn);    
    }
    else {
      return(105.0);
    }
  }
  else {
    if (!IsNTempCas()) {
      return(minTempVTOn);
    }
    if (!UpostevajElTarife()) {
        return(IzracunLimitTemp(0, minTempVTOn));  
      } 
    seRracunaHitrGret = true;
    return(IzracunLimitTemp(0, ciljnaTemp));
  }
}


//--------------------------------------------------------------------------------
float TempIzklopa(void)
{
  if (!IsWeekend()) {
    if (!IsNTempCas()) {
      return(IzracunTempVTOff());
    }
    if (!UpostevajElTarife())
        return(IzracunLimitTemp(1, minTempVTOn));    
    
    return(IzracunLimitTemp(1, ciljnaTemp));  
  }
  else {
    if (IsNTempCas()) {
      if (weekday() == NED && hour() > uraVTemp[0]) {
          return(IzracunLimitTemp(1, ciljnaTemp));  
      }  
      return(minTempNightOn + dTemp);
    }
    return(105.0); 
  }
}





//--------------------------------------------------------------------------------
//------------
static void IzpisDataOnOffSerial(void)
{
//  char ch;
  char ime[12];
  
  ImeDatotekeOnOff(ime);
  Serial.println(F(""));
  Serial.println(ime);
  
  myFile = SD.open(ime, FILE_READ);
  if (!myFile)
    Serial.println(F("error"));
  else {
      while (myFile.available()) {
        Serial.write(myFile.read());  
      }
      myFile.close();
  }    
//  myFile.close();
  Serial.println(F("")); 
  delay(50); 
}


//------------
static boolean UpostevajElTarife(void)
{
  
  float diff_factor_01 = 4.0 * dTemp;
  float diff_factor_02 = 2.0 * dTemp;
  
  if (AvgVal(sumTemp[OKOLICA_0], histLen*1.0) < (AvgVal(sumTemp[PEC_TC_DV], histLen*1.0) - diff_factor_01) && 
      (AvgVal(sumTemp[PEC_TC_DV], histLen*1.0) > (minTempVTOn + diff_factor_02 ))) {
    //minTempVTOn + dTemp
 //   minRunTimeMin = 30;
    return(false);
  }
 
//  minRunTimeMin = 45;
  return(true);
}



//--------------------------------------------------------------------------------------------

static float PovpreciVred(float a, float povVred, float lastVred) {

  return((povVred * a) + ((lastVred)*(1.0-a)));
} 

//--------------------------------------------------------------------------------------------
/*
float Sec2Hour(float sec) {

  return(sec / 3600.0);
}         
*/



