

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

#include "Kontroler_TC.h"
#include "Configuration.h"
#include "Temperature.h"

#include "Tok_napetost.h"
#include "LCD_funkcije.h"
#include "Encoder_butt.h"


static void PreklopiVentil(byte newState);
// splosne spremenljivke



unsigned long casMeritve;
unsigned long prevCasMeritve;
int prevMinute = -1;


//unsigned long startProg;




// SD spremenljivke
Sd2Card cardSD;
File myFile;


// rele spremenljivke
boolean releState_1 = R_TC_OFF;  // stanje releja vklopa TC
unsigned long lastReleChg;    // cas zadnje spremembe stanja releja TC

// LCD


int modeLCD = -1;                // slika izpisanega zaslona 
unsigned long prevLCDizp;     // cas zadnjega izpisa na LCD

// splosne spremenljivke


byte prevTCState;

unsigned long lastTCStateChg = 0;
unsigned long onTimeTC = 0;

unsigned long lastCrpTCStateChg;
unsigned long lastVentTCChg[2];

int preklopCrpTCVzr = 0;

byte prevCrpTCState = 0;
byte prevVentTCState = 255;

byte manuCrpTCState = 0;

int stateCevStPecTC;


boolean isCrpRadAsAbsTemp = true; //upobi absol. temp za zagon crpalke
//float crpRadAsAbsTemp[24];

float hitrGret = 0;
float startTemp;

char infoErr[8];

//unsigned long lastPavza;

unsigned int addrTmp;
float pTempCrp[3];

unsigned long lastRunTime;
float tempOkolicaSt;
boolean izracHitrGret = false;
boolean izracHitrGretInfo=false;
boolean seRracunaHitrGret;
float porabaWH;
float lastDeltaTh;
float lastDeltaThOk;
float lastDeltaThSt;

float zacPorabaWH;
float Qv;
float We;






/*
const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "Service Temporarily Unavailable"
  "</title></head>"
  "<body>"
    "<h3>This service is currently unavailable</h3>"
    "<p><em>"
      "The main server is currently off-line.<br />"
      "Please try again later."
    "</em></p>"
  "</body>"
"</html>"
;
*/

static word homePage() {
  /*int t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  */
  
  bfill = ether.tcpOffset();
  
  char t0[6];
  char t1[6];
  char t2[6];
  char t3[6];
  char t4[6];
  char t5[6];
  char t6[6];
  dtostrf(cTemperatura[0], 2, 2, t0);
  dtostrf(cTemperatura[1], 2, 2, t1);
  dtostrf(cTemperatura[2], 2, 2, t2);
  dtostrf(cTemperatura[3], 2, 2, t3);
  dtostrf(cTemperatura[4], 2, 2, t4);
  dtostrf(cTemperatura[5], 2, 2, t5);
  dtostrf(cTemperatura[6], 2, 2, t6);
  
  char itime[17];
  sprintf(itime, "%02d.%02d.%d %02d:%02d", day(), month(), year(), hour(), minute());
  
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='59'/>"
    "<title>Info server</title>"
    "<h1> $S</h1>"
    "<h3> T0: $SC <br> T1: $SC <br> T2: $SC <br> T3: $SC <br> T4: $SC <br> T5: $SC <br> T6: $SC</h3>"),
      itime, 
      t0, t1, t2, t3, t4, t5, t6);
  return bfill.position();
}
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// main function to print information about a device
static void printData()
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



//--------------------------------------------------------------------------------
static void PrintTempAllSDbin(void)
{

  char ime[12];
  
  ImeDatoteke(ime);
  
  myFile = SD.open(ime, FILE_WRITE);
  if (!myFile) {
      sprintf(infoErr," ErrF01");
      Serial.print(infoErr);     
  }
  else {
    NarediTimeStr(ime, now());
    myFile.print(ime);

    myFile.print(F(" "));
    myFile.print(casMeritve);
    
    for (int i=0; i<numSens; i++) { 
      myFile.print(F(" "));
      myFile.print(cTemperatura[i]);
      
    }
    myFile.print(F(" "));
    if (releState_1 == R_TC_ON) {
      if (prevTCState == 1) 
        myFile.print(F("ON"));
       else
         myFile.print(F("SB"));
     }
     else
       myFile.print(F("OFF"));
    
    
    myFile.print(F(" "));
    myFile.println(AC_mimax(), 3); 
    
    
    myFile.close();
    

  } 
  delay(100);  
}

//-------------------------------------------------------------------------
static void ZapisiOnOffSD(int state, byte tipSpremembe = 0)
{
  // tipSpremembe:
  // * 0 - vklop/izklop TC
  // * 1 - vklop/izklop ventila TC
  // * 2 - vklop/izklop crpalke TC
  // * 10 - vklop/izklop crpalke rediatorjev
  // * 100 - hitrost gretja
  
  // state:
  // * 0 - izklop naprave
  // * 1 - vklop naprave
  // * 10 - info
  
  
  char ime[12];
  ImeDatotekeOnOff(ime);
  delay(10);
  
  myFile = SD.open(ime, FILE_WRITE);
  delay(5);
  if (!myFile) {
    sprintf(infoErr," ErrF02");
    NarediTimeStr(ime, now());
    Serial.print(F(""));
    Serial.print(ime);
    Serial.print(F(" "));
    Serial.print(infoErr);    
  }  //if (!myFile)
  else {
    if (tipSpremembe == 0) {
      if (state == 1) {
        myFile.print(F("Vklop: "));
        NarediTimeStr(ime, lastTCStateChg);
        myFile.print(ime);

    
        myFile.print(F("  "));
        myFile.print(startTemp);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[okolica_0]);
        myFile.println(F(")"));
      } //state == 1
      else if (state == 0) {
        myFile.print(F("Izklop: "));
        NarediTimeStr(ime, now());
        myFile.print(ime);
        myFile.print(F("  "));
        
        myFile.print(RefTemp());
        myFile.print(F("  On:"));
        myFile.print((now() - lastTCStateChg)/60.0); 
        myFile.print(F("min  Hitr.g: "));
        myFile.print(hitrGret);
        myFile.println(F("st/h"));
      } //state == 0
    } //tipSpremembe == 0
    else if (tipSpremembe == 100) {
      if (state == 11)
        myFile.print(F("Info - "));  
      myFile.print(F("Hitrost gretja: "));
      NarediTimeStr(ime, now());
      myFile.print(ime);
      myFile.print(F(" Nova: "));
      myFile.print(deltaTh, 3);
      myFile.print(F(" Zadnja: "));
      myFile.println(lastDeltaTh, 3);
      
      myFile.print(F(" Fact.komp.ok.: "));
      myFile.print(deltaThOk, 4);
      myFile.print(F(" zadnja: "));
      myFile.print(lastDeltaThOk, 4);
      myFile.print(F(" Komp.ok ("));
      myFile.print(tKompOK-5.0);
      myFile.print(F(", "));
      myFile.print(tKompOK+5.0);
      myFile.print(F(", "));
      myFile.print(tKompOK+10.0);
      myFile.print(F(")= "));
      myFile.print(KompenzacijaTempOkolice(tKompOK-5.0), 4);
      myFile.print(F(", "));
      myFile.print(KompenzacijaTempOkolice(tKompOK+5.0), 4);
      myFile.print(F(", "));
      myFile.println(KompenzacijaTempOkolice(tKompOK+10.0), 4);
      
      myFile.print(F(" Fact.zac.temp: "));
      myFile.print(deltaThSt, 4);
      myFile.print(F(" Zadnja: "));
      myFile.print(lastDeltaThSt, 4);
      myFile.print(F(" Komp.st.temp. ("));
      myFile.print(tKompSt-20.0);
      myFile.print(F(", "));
      myFile.print(tKompSt-10.0);
      myFile.print(F(", "));
      myFile.print(tKompSt+10.0);
      myFile.print(F(")= "));
      myFile.print(KompenzZacTemp(tKompSt-20.0), 4);
      myFile.print(F(", "));
      myFile.print(KompenzZacTemp(tKompSt-10.0), 4);
      myFile.print(F(", "));
      myFile.print(KompenzZacTemp(tKompSt+10.0), 4);
      
      myFile.println("");
      
      myFile.print(F(" Zac. Poraba kVA: "));
      myFile.print(zacPorabaWH/1000.0, 3);
      myFile.print(F(" Uprabljeno (MWs): "));
      myFile.print(We/1000000.0, 3);
      myFile.print(F(" Dobljeno (MJ): "));
      myFile.print(Qv/1000000.0, 3);
      myFile.print(F(" Cop: "));
      myFile.print(Cop(),3);
      
      myFile.println("");
    }  
    else  {
      if (state == 1) {
        
        if (tipSpremembe == 1)
          myFile.print(F("Odp. ventila: "));
        else if(tipSpremembe == 2)
          myFile.print(F("Vkl. crpalke TC: "));
        else if(tipSpremembe == 10) 
          myFile.print(F("Vkl. crpalke rad.: "));
          
        NarediTimeStr(ime, now());
        myFile.print(ime);

        myFile.print(F("  "));
        myFile.print(cTemperatura[crpalka_0]);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[pec_dv]);
        myFile.print(F(" - "));
        myFile.print(cTemperatura[pec_TC_dv]);
        myFile.println(F(")"));  
      }      //state == 1
      else {
        if (tipSpremembe == 1)
          myFile.print(F("Zap. ventila: "));
        else if (tipSpremembe == 2)
          myFile.print(F("Izkl.crpalke TC: "));
        else if(tipSpremembe == 10) 
          myFile.print(F("Izkl. crpalke rad.: "));
          
        NarediTimeStr(ime, now());
        myFile.print(ime);
    
        myFile.print(F("  "));
        myFile.print(cTemperatura[crpalka_0]);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[pec_dv]);
        myFile.print(F(" - "));
        myFile.print(cTemperatura[pec_TC_dv]);
        myFile.println(F(")"));  
      } //else
    }  //else (state = 1)
    myFile.close();
  }
  delay(100);
}



//--------------------------------------------------------------------------------
static void ImeDatoteke(char* ime)
{
    
    sprintf(ime, "%04d%02d%02d.dat", year(), month(), day());
    
}

//--------------------------------------------------------------------------------
static void ImeDatotekeOnOff(char* ime)
{
    sprintf(ime, "%02d%02d%02d.dat", year()-2000, month(), day());
}

void IzpisHex2(int num) {
  if (num < 0x10)
    lcdA.print(F("0"));  
  lcdA.print(num,HEX);
}
//--------------------------------------------------------------------------------
void setup(void)
{
  char infoSet[8];

//  boolean exist = true;
//  DeviceAddress tmpAddr;
  
  // init portov
  pinMode(BEEP_PIN, OUTPUT);
  
  pinMode(RELE_TC, OUTPUT);
  digitalWrite(RELE_TC, releState_1);
  
  pinMode(RELE_CTC, OUTPUT);
  PreklopiCrpalkoTC(0);
  
  pinMode(RELE_CRAD, OUTPUT);
  digitalWrite(RELE_CRAD, R_CRAD_OFF);
  
  pinMode(VENTTC_1, OUTPUT);
  pinMode(VENTTC_2, OUTPUT);
  pinMode(VENTTC_EN, OUTPUT);
  digitalWrite(VENTTC_EN, HIGH);
  
  pinMode(SD_CS_PIN, OUTPUT);
  
  pinMode(CEVTERM_PEC_TC, INPUT_PULLUP);
  
  Serial.begin(57600);
  Serial.println(VERSION);
  
  
    //-------------------
  Wire.begin();
  delay(250);
  lcdA.begin(20, 2);
  lcdA.print(F(" LCD OK"));
  analogWrite(LCD_OSW_SW, 255);
  //-------------------
  if (ether.begin(sizeof Ethernet::buffer, mymac, ETHER_CS_PIN) == 0) 
    Serial.println( "Failed to access Ethernet controller");
 
  if (!ether.staticSetup(myip, gwip, dnsip)) {
    Serial.println(F("Preveri povezavo ethernet modula!"));
  }  
  
  
    
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip); 
  
//  Serial.println(ether.packetLoopIcmpCheckReply(gwip));	
//  ether.ntpRequest(ntpip, 8888); 
//  uint32_t *t;
//  Serial.println(ether.ntpProcessAnswer(t, 0)); 		
   
  

  
  delay(250);
 
  //Dallas temp. senzor init  
  // locate devices on the bus
  // Trenuino samo fiksne addrese
  
  numSens = FiksAdrrSens(devAddress, type_s);
  

  Serial.print(numSens, DEC);
  Serial.println(F(" devices."));
  for (int i=0; i<numSens; i++) {
    PrintAddress(devAddress[i]);
//    if (OneWire::crc8(devAddress[i], 7) != devAddress[i][7])
//       type_s[i] = 255; 
    Serial.print(F(" "));
    Serial.println(type_s[i]);   
  }


boolean exist = true;
DeviceAddress tmpAddr;

  while (exist == true) {
    exist = ds.search(tmpAddr);
    if (exist == false) {
      ds.reset_search();
      delay(250);
      break;
    }  
    if (OneWire::crc8(tmpAddr, 7) != tmpAddr[7]) {
      sprintf(infoErr," ErrT01 ");
    }  
    else {
//      PrintAddress(tmpAddr);
    }  
        
  }

  
  if (numSens > MAXSENSORS)
    numSens = MAXSENSORS;
  
  for (int i=0; i<numSens; i++) {
    lcdA.clear();
    lcdA.print(F("T "));
    if (i<10) {
      lcdA.print(F("0"));
    }  
    lcdA.print(i);
    lcdA.print(F(":"));
    
    lcdA.setCursor(0, 1);
    IzpisHex2(devAddress[i][0]);
//    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][1]);
    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][2]);
//    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][3]);
    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][4]);
//    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][5]);
    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][6]);
//    lcdA.print(F(" "));
    IzpisHex2(devAddress[i][7]);
    lcdA.print(F(" "));
    
    PreberiTemperaturo(i, false);
    delay(convWaitTime);
    cTemperatura[i] = PreberiTemperaturo(i, true);
    lcdA.setCursor(7, 0);
    lcdA.print(cTemperatura[i],1);
    delay(2000);
  }
  
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
   
  
  // inicializacija SD kartice
//  Serial.println("SD init..");
  lcdA.clear();
  lcdA.print(F("SD Card:"));
  lcdA.setCursor(0, 1);
/*  if (!cardSD.init(4, chipSelectSD)) {
    sprintf(infoErr," Err SDi");
    Serial.print(infoErr);   
    lcdA.print(infoErr);
  } 
*/  
  if (!SD.begin(SD_CS_PIN)) {
    sprintf(infoErr," Err SD");
    Serial.print(infoErr);   
    lcdA.print(infoErr);

  } 

  else {
    lcdA.print(F(" SD OK"));
  }
  delay(2000);
  
  lcdA.clear();
  lcdA.print(F("Free RAM"));
  lcdA.setCursor(1, 1);
  lcdA.print(FreeRam());
  
  Serial.print(F("Free RAM: "));
  Serial.println(FreeRam()); 

  
  
//  prevCasMeritve = 0;


  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrOnTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4) );
  delay(5);
  onTimeTC = u4.ulval;
  Serial.print(F("OnTime: "));
  Serial.print(onTimeTC);
  Serial.println(F("s"));

  delay(5); 
  
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
  delay(5);
  lastTCStateChg = u4.ulval;
  Serial.print(F("Last TC chg: "));
  NarediTimeStr(infoSet, lastTCStateChg);
  Serial.print(day(lastTCStateChg));
  Serial.print(F("."));
  Serial.print(month(lastTCStateChg));
  Serial.print(F("."));
  Serial.print(year(lastTCStateChg)); 
  Serial.print(F("  "));
  Serial.print(infoSet);
  
  
 // i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
  prevTCState = i2c_eeprom_read_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH);
  if (prevTCState != 0) {
    prevTCState = 0;
    lastTCStateChg = now();
    u4.ulval = lastTCStateChg;
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
    i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
    Serial.print(F(  "\"prevTCState\" nastavljeno na "));
    Serial.println(prevTCState);
  } 
  else {
    Serial.println();
  }  
  
  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaTh = uf.fval;
  Serial.print(F("Hir. gret: "));
  Serial.print(deltaTh, 3);
  if (deltaTh <= 0 || deltaTh > 10.0) {
    deltaTh = 4.35;
    Serial.print(F(" / "));
    Serial.print(deltaTh, 3);
    uf.fval =  deltaTh;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaTh, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  }
  Serial.println(F("K/h"));
  
 i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaThOk = uf.fval;
  Serial.print(F("Komp.ok.: "));
  Serial.print(deltaThOk, 4);
  if (abs(deltaThOk) > 100.0) {
    deltaThOk = 0.0;
    Serial.print(F(" / "));
    Serial.print(deltaThOk, 4);
    uf.fval =  deltaThOk;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThOk, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  }
  Serial.println(F("")); 

  Serial.print(F("Kompenz. temp. okolice pri temp. "));
  Serial.print(cTemperatura[okolica_0]);
  Serial.print(F("C je "));
  Serial.println(KompenzacijaTempOkolice(cTemperatura[okolica_0]),4);
  

  i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  delay(5);
  deltaThSt = uf.fval;
  Serial.print(F("Komp.st.: "));
  Serial.print(deltaThSt, 4);
  if (abs(deltaThSt) > 100.0) {
    deltaThSt = 0.0;
    Serial.print(F(" / "));
    Serial.print(deltaThSt, 4);
    uf.fval =  deltaThSt;
    delay(5);
    i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrDeltaThSt, AT24C32_ADDR_LENGH, (byte *)&uf, sizeof(uf));
  }
  Serial.println(F("")); 
  
  Serial.print(F("Kompenz. zacetne temp. pri temp. "));
  Serial.print(cTemperatura[crpalka_0]);
  Serial.print(F("C je "));
  Serial.println(KompenzZacTemp(cTemperatura[crpalka_0]),4);

  
  for (int j = 0; j < numSens; j++) {
    Serial.print(F("Addres:"));
    Serial.print(addrTempBack + ((j*histLen))*sizeof(u2));
    Serial.print(F("  "));
    for (unsigned int i=0; i < histLen; i++) {   // 7*1440/marXMin
      addrTmp =  addrTempBack + (i + (j*histLen))*sizeof(u2);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);
      
      if ((u2.uival) < 100 || (u2.uival) > 12000) {
        if (i > 0)
          u2.uival = (sumTemp[j] * 100) /(i);
        else
          u2.uival = 5000;
        delay(2);

        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      sumTemp[j] += (float)(u2.uival/100.0);
      delay(10);
    }
    Serial.print(j+1);
    Serial.print(F(": "));
    Serial.print(AvgVal(sumTemp[j], histLen*1.0), 3); 
    Serial.println(F(" "));  
  }    
  
//  NastaviTempCrpRad(limTempCrpRad, limTempFactCrpRad, crpRadAsAbsTemp);
  temeratureIzmerjene = true;
  prevCasMeritve = now();
  casMeritve = now();
  
  PreveriNapetosti(true, true, true);
  
  
  
  
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
   
//   do {
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
//    temeratureIzmerjene = PreberiTemperatureDS(false, true);
    PreberiTemperatureDS(false, true);
    temeratureIzmerjene = false;
    casMeritve = now();
    
  }
  if ((now() >= (casMeritve + convWaitTime/1000)) && (temeratureIzmerjene == false)) {
//  if (now()/(merXMin*60) > prevCasMeritve/(merXMin*60)) {  /* || (minute() < 30 && prevMinute > 30)*/
//    temeratureIzmerjene = PreberiTemperatureDS(true, false);
    PreberiTemperatureDS(true, false);
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
    printData();
    
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
        if (j == crpalka_0) {
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
          u2.uival =  cTemperatura[j]*100;
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
      for (int i=0; i<numSens; i++) {
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

      PreveriNapetosti(false, true, true);
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
     Serial.print(F(") "));
     Serial.print(osvetlitevLCD);

    

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
          tempOkolicaSt = cTemperatura[okolica_0];
          u4.ulval = lastTCStateChg;
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastChg, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
          i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
          
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
        i2c_eeprom_write_byte(AT24C32_I2C_ADDR, addrLastChg+4, AT24C32_ADDR_LENGH, prevTCState);
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
    
  if (now()/5 > prevLCDizp) {
    prevLCDizp = now()/5;
    IzpisiNaLCD();
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
    if (max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) > cTemperatura[crpalka_0] + minDiffTCPec) {
      if (max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) > tempVklopaCrpTC) {
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
    if ((stateCevStPecTC == CEV_TERM_ON) && (cTemperatura[crpalka_0] < cTemperatura[pec_dv])) {
        preklopCrpTCVzr = 2;  // cevni termostat vklople
        return(1);
     }
     if (!IsCasTransfTopl()) {
       if (cTemperatura[pec_dv] < (tempIzklopaVentCrpTC - 5.0)) { 
//       if (min(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) < tempIzklopaVentCrpTC - 5.0) {   
         if (cTemperatura[pec_dv] > (tempIzklopaVentCrpTC - 12.5)) {
//         if (max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) > tempIzklopaVentCrpTC - 12.5) {  
           if (cTemperatura[crpalka_0] > tempIzklopaVentCrpTC) {
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
      if (stateCevStPecTC == CEV_TERM_OFF || (cTemperatura[crpalka_0] > cTemperatura[pec_dv])) {
        return(0);
      }
    }
    else if (preklopCrpTCVzr == 4) {
      if ((cTemperatura[crpalka_0] <= tempIzklopaVentCrpTC) || 
          (min(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) > cTemperatura[crpalka_0]) || 
          (max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) < tempIzklopaVentCrpTC - 12.5))  {
        return(0);
      }
    } 
    else {  
      if ((cTemperatura[crpalka_0] >= max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv])) && 
            ((cTemperatura[crpalka_0] <= TempIzklopaCrpTC() || MaxCrpTCRunTime()) || IsNTempCas())) {
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
        if ((max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) <= TempIzklopaCrpTC())  && cTemperatura[crpalka_0] <= TempIzklopaCrpTC()) {
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
  
//  dTemp = (float) abs((float) pTempCrp[2] - (float) cTemperatura[crpalka_0]);
//  dTemp += ((float) abs((float) pTempCrp[1] - (float) cTemperatura[crpalka_0]));
  
  if (cTemperatura[pec_TC_dv] < tempVklopaCrpTC) {
    return(false); 
  } 
  
  dTemp = sq(pTempCrp[2] - cTemperatura[crpalka_0]);
  dTemp += (sq(pTempCrp[1] - cTemperatura[crpalka_0]));
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
          if (cTemperatura[pec_TC_dv] > tempVklopaCrpTC) {
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
          if (cTemperatura[pec_TC_dv] > tempVklopaCrpTC) {
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
          if (cTemperatura[crpalka_0] >= cTemperatura[pec_dv]) {
            PreklopiVentil(0);
          }
        } else {              // !IsCasTransfTopl()
          if (preklopCrpTCVzr == 0) {
            if (cTemperatura[crpalka_0] < tempIzklopaVentCrpTC) {
              PreklopiVentil(0);
            }
          }
          else if (preklopCrpTCVzr == 1) {
            if (RefTemp() > TempIzklopaCrpTC_NTemp()) {
              PreklopiVentil(0);
            }
          }
          else if (preklopCrpTCVzr == 2) {
            if ((stateCevStPecTC == CEV_TERM_OFF) || (cTemperatura[crpalka_0] > cTemperatura[pec_dv])) {
              PreklopiVentil(0);
            }
          }  
        }
      }
    }
    if (prevVentTCState == 0) {
      if (!IsCasTransfTopl() && !IsNTempCas()) {
        if (cTemperatura[crpalka_0] > tempIzklopaVentCrpTC) {
          if (max(cTemperatura[pec_dv], cTemperatura[pec_TC_dv]) > minTempPecPonovnoOdpVent) {
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
    if (cTemperatura[crpalka_0] + minDiffTCPec > tempIzklopaVentCrpTC || prevCrpTCState == 1) {
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
static void PreklopiCrpalkoTC(byte newState)
{
  char cas[13];
  
//  if (newState < 255) {
    
    if (newState == 1) {
      if (preklopCrpTCVzr == 0) {
        if (cTemperatura[pec_TC_dv] >= tempVklopaCrpTC || cTemperatura[pec_dv] >= tempVklopaCrpTC) {
          if ((cTemperatura[pec_pv] < tempVklopaCrpTC - 3.0*dTemp) && (cTemperatura[rad_pv] < tempVklopaCrpTC - 3.0*dTemp)) {
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
    if (cTemperatura[pec_dv] - cTemperatura[okolica_0] > minDiffDvOkolCrpRad) {
      if (cTemperatura[rad_pv] < MejnaTempPreklCrpRad(1)) {
        if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
          PreklopiCrpalkoRad(1);
        }
      }
    }
    if (cTemperatura[rad_pv] > (cTemperatura[rad_dv] + 2.0*dTemp)) {
      if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
          PreklopiCrpalkoRad(1);
      }  
    }  
  }
  else {
    if (RelaksTimeLimitSec(now(), lastCrpRadStateChg, minCrpRadRunTimeMin*60)) {
      if (cTemperatura[rad_pv] >= MejnaTempPreklCrpRad(0)) { 
        PreklopiCrpalkoRad(0);  
      }
      else if (cTemperatura[pec_dv] - cTemperatura[okolica_0] < minDiffDvOkolCrpRad) {
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
  
  refTemp = (float)limTempCrpRad[hour()] + limTempFactCrpRad[hour()] * cTemperatura[rad_dv];
  if (newState == 1) {
    tmpTemp = cTemperatura[rad_dv] - refTemp;
    if (isCrpRadAsAbsTemp) {
      zeljTemp =  (float)crpRadAsAbsTemp[hour()] - 10.0;
 
        dTemp = (zeljTemp - cTemperaturaZun) * kTemp; 
 //       if (cTemperatura[ref_prostor_1] < zeljTemp) {
        dTemp *= (zeljTemp / cTemperatura[ref_prostor_1]);
 //     }
      tmpTemp = min((float)crpRadAsAbsTemp[hour()] + dTemp, tmpTemp);
    }
    if (cTemperatura[pec_dv] >  maxTempDVPec) {
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
    if (cTemperatura[rad_dv] - refTemp > maxTempPVRad)
      return(maxTempPVRad);
    
    if (cTemperatura[rad_dv] - refTemp < 5.00)  {
      return(5.0);
    }  
    return(cTemperatura[rad_dv] - refTemp);
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
static float RefTemp()
{
 // return(AvgAllTimeTemp(crpalka_0));
  return(cTemperatura[crpalka_0]);
}

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
  
  
//refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) / KompenzacijaTempOkolice(cTemperatura[okolica_0]);  
  
//  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0)));
//  refTemp *= KompenzacijaTempOkolice(cTemperatura[okolica_0]);
//  refTemp *= KompenzZacTemp(cTemperatura[crpalka_0]);
  refTemp = ciTemp - deltaTh*(tUra + (1.0 - (minute()/60.0))) * KompenzacijaTempOkolice(cTemperatura[okolica_0]) * KompenzZacTemp(cTemperatura[crpalka_0]);  


  
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

static float AvgVal(float suma, float num)
{
  return(suma/num);
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
  
  if (AvgVal(sumTemp[okolica_0], histLen*1.0) < (AvgVal(sumTemp[pec_TC_dv], histLen*1.0) - diff_factor_01) && 
      (AvgVal(sumTemp[pec_TC_dv], histLen*1.0) > (minTempVTOn + diff_factor_02 ))) {
    //minTempVTOn + dTemp
 //   minRunTimeMin = 30;
    return(false);
  }
 
//  minRunTimeMin = 45;
  return(true);
}


//--------------------------------------------------------------------------------------------
float KompenzacijaTempOkolice(float tOkolice)
{
//  return(1 + (20.0 - tOkolice) * deltaThOk);
  float komp;
  
  if (tKompOK == 0) {
    komp = 1.0;
  }
  else {
    komp = (((tOkolice-tKompOK)/tKompOK)*deltaThOk);
    komp = 1.0 + komp;
//    komp = (1.0 - (deltaThOk*((tOkolice-tKomp0)/tKomp0)));  
  }  
  
   return(komp);
}

//--------------------------------------------------------------------------------------------
float KompenzZacTemp(float tStart)
{
  float komp;
  float sqtStart;
  float sqtKompST;
 
 /* 
  if (tStart <= tKompSt) {
//    komp = 1.0;
    return(1.0);    
  }  
*/
  if (abs(tKompSt) < 0.1) {
//    komp = 1.0;
    return(1.0);
  }  
  else {
    sqtStart = tStart*tStart;
    sqtKompST = tKompSt*tKompSt;
 //   komp = tKompSt*tKompSt - sqtStart;
 //   komp = komp / (sqtStart);
 //   komp = komp * deltaThSt;
   
  ////  komp = (((tKompSt*tKompSt - tStart*tStart)/tStart*tStart)*deltaThSt);
  //  komp = (1 + komp);
    
     komp = 1+ (((sqtKompST - sqtStart)/sqtKompST)*deltaThSt);
  }
  return(komp); 
}


//--------------------------------------------------------------------------------------------

float IzracDeltaTh() {

  float imenovalec;
  
  imenovalec = (KompenzacijaTempOkolice(tempOkolicaSt) * KompenzZacTemp(startTemp) * (Sec2Hour(lastRunTime)));
  
  if (abs(imenovalec) < 0.001)
    return(deltaTh);
  return((cTemperatura[crpalka_0]- startTemp) / imenovalec);
  
}  


//--------------------------------------------------------------------------------------------
float IzracDeltaThOk() {
  float stevec;
  float imenovalec;
  float a;
  
//  stevec = (startTemp - cTemperatura[crpalka_0] + deltaTh* Sec2Hour((float)lastRunTime) * KompenzZacTemp(startTemp));
//  stevec = stevec * tKomp0;
  
  a = deltaTh*Sec2Hour(lastRunTime)*KompenzZacTemp(startTemp);
  stevec = (cTemperatura[crpalka_0] - startTemp - a) * tKompOK;
  imenovalec = a * (tempOkolicaSt-tKompOK);
  
//  stevec = (startTemp - cTemperatura[crpalka_0] + deltaTh*Sec2Hour(lastRunTime)*KompenzZacTemp(startTemp)) * tKompOK;

//  imenovalec = (tempOkolicaSt-tKomp0);
//  imenovalec *= deltaTh;
//  imenovalec *= Sec2Hour((float)lastRunTime);
//  imenovalec *= KompenzZacTemp(startTemp);

//  imenovalec = deltaTh*Sec2Hour(lastRunTime)*(tempOkolicaSt-tKompOK)*KompenzZacTemp(startTemp);
  
//  if (abs(imenovalec) > 0.01)
//  if (imenovalec < 0.01 && imenovalec > -0.01)
  if (abs(imenovalec) < 0.0001)
    return(deltaThOk);
  
  stevec = stevec/imenovalec;
  
  if (stevec > 0) {
    if (deltaThOk > 0.5 && stevec > maxDeltaDev * deltaThOk) {
      return(maxDeltaDev * deltaThOk);  
    } 
    return(stevec);
  }
  return(0.0);  
}   




//--------------------------------------------------------------------------------------------
float IzracDeltaThSt() {
  float stevec;
  float imenovalec;
  
  float sqtKompSt = startTemp*startTemp;
  float sqtKompStRef = tKompSt*tKompSt;
   
 // stevec = cTemperatura[crpalka_0] - startTemp - (deltaTh * Sec2Hour((float)lastRunTime) * KompenzacijaTempOkolice(tempOkolicaSt)); 
 // stevec = stevec * sqtKompSt; 
  
  stevec = (cTemperatura[crpalka_0] - startTemp - deltaTh * Sec2Hour(lastRunTime) * KompenzacijaTempOkolice(tempOkolicaSt)) * sqtKompStRef;
  
 // imenovalec = (tKompSt*tKompSt - sqtKompSt);
 // imenovalec *= deltaTh;
 // imenovalec *= Sec2Hour((float)lastRunTime);
 // imenovalec *= KompenzacijaTempOkolice(tempOkolicaSt);

  imenovalec = deltaTh*Sec2Hour(lastRunTime)*(sqtKompStRef - sqtKompSt)*KompenzacijaTempOkolice(tempOkolicaSt);
  
 // if (abs(imenovalec) > 0.01)
 //   return(stevec/imenovalec);
 // return(deltaThSt);  
  if (abs(imenovalec) < 0.0001)
    return(deltaThSt);
  
  stevec = stevec/imenovalec;
  if (stevec > 0) {
    if (deltaThSt > 0.5 && stevec > maxDeltaDev * deltaThSt) {
      return(maxDeltaDev * deltaThSt);  
    }  
    return(stevec);
  }
  return(0.0);    
  
    

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

static float Cop(void)
{
  //float Q;
  //float W;
  /*
  c = 4200; //J/kgK
  m = 230; //kapaciteta bojlerja 
  
  dT = cTemperatura[crpalka_0] - startTemp;
  Q = c * m * dT;
  W = (porabaWh - zacPorabaWh) * 3600; //VAh *3600
  */
  Qv = 4200.0 * 230.0 * (cTemperatura[crpalka_0] - startTemp);
  We = (porabaWH - zacPorabaWH) * 3600.0;
  if (We > 0)
    return(Qv/We);
  return(0);  
}
