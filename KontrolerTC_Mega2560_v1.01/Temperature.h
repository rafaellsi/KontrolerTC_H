#ifndef Temperature_h
#define Temperature_h


#include <DHT.h>

//deklaracije zunanjih funkcij
extern  void IzpisHex2(int num);
extern void Beep(unsigned char delayms);

//deklaracije internih funkcij
void TempSensorsInit(void);
static float PreberiTemperaturoDS(int cSens, boolean zahtevajBranje);
static boolean PreberiTemperature(boolean zahtevajBranje, boolean allSens);

float PreberiTemperaturoDHT22(int n);
float PreberiVlaznostDHT22(int n);
float IzracunTRosicsa(float temp, float vlaz);
float IzracunHumidex(float temp, float tempRos);

float PreberiTemperaturoK(int n);

void PrintAddress(DeviceAddress deviceAddress);
void PrintTemperatureAll(void);

time_t processSyncMessage();
float AutoTimeUnitConv(unsigned long cas, char *cunits);
boolean IsNTempCas();
boolean IsCasTransfTopl();

void last24H_Info(void);
unsigned int ObsegZgodovine(int sensor, unsigned int pred);
float RefTemp(byte refTempType);



//definicija spremenljivk
DeviceAddress devAddress[MAXSENSORS_DS];  //
unsigned long convWaitTime = 800;  //1000
boolean temeratureIzmerjene=true;

float avgTempVodeTC;
//float refTempIzrac = -1.0;

//--------------------------------------------------------------------------------
// Inicializacija temp. senzorjev
void TempSensorsInit(void) {
  
  //Dallas temp. senzor init  
  // locate devices on the bus
  // Trenuino samo fiksne addrese
  
  
  // incializacija DS18x20 temp. .. senzorjev
  FiksAdrrSens(devAddress, type_s);
  Serial.println(F(""));
  Serial.print(numSensDS, DEC);
  Serial.println(F(" devices."));
  for (int i=0; i<numSensDS; i++) {
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
      PrintAddress(tmpAddr);  //dodaj preverjanje address
    }  
        
  }

  numSens = numSensDS + numSensDHT22 + numSensK; 
  if (numSens > MAXSENSORS) {
    Beep(200);
    Serial.println(F(""));
    Serial.println(F("Stevilo temp. senzorjev se ne ujema!!"));
    lcdA.clear();
    lcdA.print(F("Stevilo senzorjev"""));
    Beep(200);
    numSensDS = MAXSENSORS - numSensDHT22 - numSensK;
  }
  
  for (int i=0; i<numSensDS; i++) {
    lcdA.clear();
    lcdA.print(F("T "));
    if (i<10) {
      lcdA.print(F("0"));
    }  
    lcdA.print(i);
    lcdA.print(F(":"));
    
    lcdA.setCursor(0, 1);
    for (int j=0; j<8; j++) {
      IzpisHex2(devAddress[i][j]);
      if (j%2 == 1)
        lcdA.print(F(" "));    
    }
    
     
/*  
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
*/    
    PreberiTemperaturoDS(i, false);
    delay(convWaitTime);
    cTemperatura[i] = PreberiTemperaturoDS(i, true);
    lcdA.setCursor(7, 0);
    lcdA.print(cTemperatura[i],1);
    delay(2000);
  }
  
  // incializacija ost. temp. .. senzorjev
  dht.begin();
  for (int i=numSensDS; i<numSensDS + numSensDHT22; i++) {
    cTemperatura[i] = PreberiTemperaturoDHT22(i-numSensDS);
    if (cTemperatura[i] < -100) {
      Serial.print(F("Napaka DHT22 senzorja!!"));
      lcdA.print(F("Napaka DHT22 senzorja!!"));
    }
    lcdA.clear();
    lcdA.print(F("T "));
    if (i<10) {
      lcdA.print(F("0"));
    }  
    lcdA.print(i);
    lcdA.print(F(":"));
    
    lcdA.setCursor(0, 1);
    lcdA.print(F(" DHT22 senzor"));
        lcdA.setCursor(7, 0);
    lcdA.print(cTemperatura[i],1); 
    delay(2000);    
  }
  for (int i=numSensDS + numSensDHT22; i<numSensDS + numSensDHT22 + numSensK; i++) {
    cTemperatura[i] = PreberiTemperaturoK(i-numSensDS-numSensDHT22);
    lcdA.clear();
    lcdA.print(F("T "));
    if (i<10) {
      lcdA.print(F("0"));
    }  
    lcdA.print(i);
    lcdA.print(F(":"));
    
    lcdA.setCursor(0, 1);
    lcdA.print(F(" K-type senzor"));
    lcdA.setCursor(7, 0);
    lcdA.print(cTemperatura[i],1);
    delay(2000);    
  }
  avgTempVodeTC = cTemperatura[CRPALKA_0]; 
}  





//--------------------------------------------------------------------------------
// branje senzorja
static float PreberiTemperaturoDS(int cSens, boolean zahtevajBranje)
{
  byte present = 0;
  unsigned int raw;
  byte data[12];
  int errDS = 0;
  float cTemp;
  
  
  if (!zahtevajBranje) {
    ds.reset();
    if (cSens == MAXSENSORS) {
      ds.write(0xCC);
    }
    else {
      ds.select(devAddress[cSens]);
    }
    ds.write(0x44,0);         // start conversion 
  }
  //  delay(convWaitTime);
  else {
    present = ds.reset();
    ds.select(devAddress[cSens]);    
    ds.write(0xBE);         // Read Scratchpad

  
    for (int  i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
      if (data[i] == 0) {
          errDS++; 
      }
    }
  
     
/*  
  if (showCRC) {
    Serial.println();
    Serial.print(present,HEX);
    Serial.print(" - ");
    for (int i = 0; i < 9; i++) {           // we need 9 bytes     
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
  }



*/
  if (OneWire::crc8(data, 8) != data[8]) {
      errDS += 8;;
  }

  // convert the data to actual temperature
    raw = (data[1] << 8) | data[0];
    if (type_s[cSens]) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // count remain gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
  
    cTemp = (float)raw / 16.0;
  
    if (cTemp > 1000 || errDS >= 8) {
      Serial.println(F(""));
      Serial.print(F(" D"));
      Serial.print(cSens+1);
      for (int i=0; i< 9; i++) {
        Serial.print(F(" "));
        Serial.print(data[i],HEX);  
      }
      Serial.print(F(" "));
      Serial.print(cTemp,2);
      if (/*cTemp > 1000 && */errDS >= 8) {
        cTemp = 1001.0;
      }  
    }
    return(cTemp);
  }
  return(-1001.0);
  //return((float)raw / 16.0);
}




//--------------------------------------------------------------------------------
// Branje vseh temp. senzorjev
static boolean PreberiTemperature(boolean zahtevajBranje, boolean allSens = false)
{
  float cTemp;
  
  if (allSens && !zahtevajBranje) {
    cTemp = PreberiTemperaturoDS(MAXSENSORS, zahtevajBranje);
    
    for (int i=numSensDS; i<(numSensDS + numSensDHT22); i++) {
      cTemperatura[i] = PreberiTemperaturoDHT22(i-numSensDS);
      cVlaznost[i-numSensDS] = PreberiVlaznostDHT22(i-numSensDS);
//      cTempRosicsa[i-numSensDS] = IzracunTRosicsa(i-numSensDS);
      cTempRosicsa[i-numSensDS] = IzracunTRosicsa(cTemperatura[i], cVlaznost[i-numSensDS]);
//      cHumidex[i-numSensDS] = IzracunHumidex(i-numSensDS);
      cHumidex[i-numSensDS] = IzracunHumidex(cTemperatura[i], cTempRosicsa[i-numSensDS]);
    }
    for (int i=(numSensDS + numSensDHT22); i<(numSensDS + numSensDHT22 + numSensK); i++) {
      cTemperatura[i] = PreberiTemperaturoK(i - numSensDS + numSensDHT22);  
    }  
  }
  else {
    for (int i=0; i<numSensDS; i++) {
      cTemp = PreberiTemperaturoDS(i, zahtevajBranje);
      if (cTemp < -1000.0) {
        continue;
      }  
      else if (cTemp < 1000.0) {
        cTemperatura[i] = cTemp;
      }
      else {
//      cTemperatura[i] = AvgVal(sumTemp[i], histLen*1.0);
        Serial.print(F(" CEr"));
        Serial.print(i+1);
        Serial.print(F(" "));
      }
    }
  }
  return(zahtevajBranje);  
}

//--------------------------------------------------------------------------------
float PreberiTemperaturoDHT22(int n) {
  float temp;
  
  temp = dht.readTemperature();
  if (isnan(temp)) {
//    return(cTemperatura[n+numSensDS]);
    return(-101.0);
  }  
  return(temp);
}

//--------------------------------------------------------------------------------
float PreberiVlaznostDHT22(int n) {
  float vlaz;
  
  vlaz = dht.readHumidity();
  if (isnan(vlaz)) {
//    return(cVlaznost[n]);
    return(-1.0);
  } 
  return(vlaz);
}

//--------------------------------------------------------------------------------
float IzracunTRosicsa(float temp, float vlaz) {
//  return(cTemperatura[n + numSensDS] - (100.0 - cVlaznost[n])/5.0); 
  return(temp - (100.0 - vlaz)/5.0); 
}

//--------------------------------------------------------------------------------
float IzracunHumidex(float temp, float tempRos) {
  
//  float humidex = cTemperatura[n + numSensDS] + 0.5555 * (6.11 * exp(5417.7530 * ((1/273.16)-(1/(cTempRosicsa[n]+273.16)))) -10.0);
  float humidex = temp + 0.5555 * (6.11 * exp(5417.7530 * ((1/273.16)-(1/(tempRos+273.16)))) -10.0);
  return (humidex);
}

//--------------------------------------------------------------------------------
float PreberiTemperaturoK(int n) {
  int numSamples = 5;
  int sumTempRaw = 0;
  int tempRaw;
  int maxTRaw;
  int minTRaw;
  
//  noInterrupts();
  tempRaw = analogRead(T_KTYP_01_PIN);
  maxTRaw = tempRaw;
  minTRaw = tempRaw;
  sumTempRaw = tempRaw;
  for (int i=1; i<numSamples; i++) {
    delay(1);
    tempRaw = analogRead(T_KTYP_01_PIN);
    sumTempRaw += tempRaw;
    if (tempRaw > maxTRaw) {
      maxTRaw = tempRaw;
    }
    else if (tempRaw < minTRaw) {
      minTRaw = tempRaw;  
    }  
  }
//  interrupts();
  sumTempRaw -= (maxTRaw + minTRaw);
  sumTempRaw /= (numSamples - 2);
  
  return((vccInternal * 100.0 * sumTempRaw/1023.0)+kTypeOffset);
}  

/*
  cTemperatura[numSensDS] = dht.readTemperature();
  cVlaznost[0] = dht.readHumidity();

  if (isnan(cTemperatura[numSensDS]) || isnan(cVlaznost[0])) {
    return;
  }  
  Serial.print(F("T"));
  Serial.print(numSensDS+1);
  Serial.print(F(":"));
  Serial.print(cTemperatura[numSensDS], 2);  
  Serial.print(F(" RH:"));
  Serial.print(cVlaznost[0], 2);
  
  float Tdp = cTemperatura[numSensDS] - (100.0 - cVlaznost[0])/5.0;
  float humidex = cTemperatura[numSensDS] + 0.5555 * (6.11 * exp(5417.7530 * ((1/273.16)-(1/(Tdp+273.16)))) -10.0);


  Serial.print(F("-Tdp:"));
  Serial.print(Tdp, 2); 
  Serial.print(F(" Hum:"));
  Serial.print(humidex, 2);
  Serial.print(F("-"));
  
  
  int numSamples = 3;
  int tempRaw = 0;
  
  noInterrupts();
  for (int i=0; i<numSamples; i++) {
    tempRaw += analogRead(T_KTYP_01_PIN);
  }
  interrupts();
  tempRaw /= numSamples;
  
  Serial.print(F("K:"));
  cTemperatura[numSensDS+1] = (vccInternal * 100.0 * tempRaw/1023.0);
  Serial.print(cTemperatura[numSensDS+1], 2);
  Serial.print(F(" ("));
  Serial.print(tempRaw);
  Serial.print(F("/"));
  Serial.print(cTemperatura[numSensDS+1] - cTemperatura[numSensDS], 2);
  Serial.print(F(")-"));

*/

//--------------------------------------------------------------------------------
// function to print a device address
void PrintAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) 
      Serial.print(F("0"));
    Serial.print(deviceAddress[i], HEX);
    Serial.print(F(" "));
  }
}


//--------------------------------------------------------------------------------
// function to print the temperature for a device
float sumCTemp=0.0;
float sumCTempEMA=0.0;
extern float vccInternal;


void PrintTemperatureAll(void)
{
  
  static float pAvgTempVodeTC = 0.0;
  unsigned int addrTmp;
  
   
  for (int i=0; i<numSensDS; i++) {
    
    Serial.print(F("T"));
    Serial.print(i+1);
    Serial.print(F(":"));
    Serial.print(cTemperatura[i], 2);
    
    if (i == CRPALKA_0) {
/*      if (sumCTemp == 0) {
        for (int j=0; j<60; j++) {
          int n=minute()-(j+1);
          if (n < 0) {
            n+=60;
          }  
          addrTmp = addrLastHourTemp + (unsigned int) n * sizeof(u2);
          i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
          delay(2);
          
          sumCTemp += (u2.uival/100.0);
          sumCTempEMA += ((u2.uival/100.0) * (60.0-j));
        }
      }
*/      
      Serial.print(F("("));
      addrTmp = addrLastHourTemp + (unsigned int) minute() * sizeof(u2);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);

//      Serial.print(cTemperatura[i] - ((u2.uival/100.0)-50.0), 2);

//      Serial.print(cTemperatura[i] - PretvotiEETemp2Float(u2.uival), 2);      
      lastHourTempChange = cTemperatura[i] - PretvotiEETemp2Float(u2.uival);
      Serial.print(lastHourTempChange, 2); 
      


     
      if (abs(u2.uival - ((cTemperatura[i]*100.0))+50.0) > 1) {
//        u2.uival = (unsigned int) ((cTemperatura[i]+0.005+50.0)*100.0);
        u2.uival = PretvoriFloat2EETemp(cTemperatura[i]);

 //       Serial.print(F("z"));
 //       Serial.print(addrTmp);
        delay(2);
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      delay(2);
/*      ui2byte2 u2test;
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2test, sizeof(u2test));
      delay(2);

      if (abs(cTemperatura[i]*100.0 - u2test.uival) > 2) {
        Serial.print(F("*"));
        Serial.print(u2test.uival);
      }
      else {
        Serial.print(F("/"));
        Serial.print(u2test.uival);
      }  
 */   
      Serial.print("/");
/*
//      float avgWeight = 0.5/(float) zamikMerTemp;
      float avgWeight = 0.25/(float) zamikMerTemp;   //spremenjeno 13.4.2015
      avgTempVodeTC = ((1.0-avgWeight)*avgTempVodeTC) + (avgWeight * cTemperatura[i]); 
      Serial.print(avgTempVodeTC);
//      Serial.print(F(")"));
*/      
      avgTempVodeTC = ((1.0-factWeightAvgTemp)*avgTempVodeTC) + (factWeightAvgTemp * cTemperatura[i]); 
      Serial.print(avgTempVodeTC);
      
      
      Serial.print("/");
      avgLHTCVodeTC = ((1.0-avgWeightLHTC)*avgLHTCVodeTC) + (avgWeightLHTC * lastHourTempChange);
      Serial.print(avgLHTCVodeTC);
      Serial.print(F(")"));
    
      if (pAvgTempVodeTC >= ciljnaTempPregrevanja) {
        if (avgTempVodeTC < ciljnaTempPregrevanja) {
          lastPregrevTime = now();
          u4.ulval = lastPregrevTime;
          delay(2);
          i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrLastPregrevTime, AT24C32_ADDR_LENGH, (byte *)&u4, sizeof(u4));
          delay(2);
          Serial.print(F("Pregrevanje OK"));  
        }  
      }    
      pAvgTempVodeTC = avgTempVodeTC;
    } 
    
//    sendPlotData(cTemperatura[i], 2);
    
    Serial.print(F("-"));
  }
  
  for (int i=numSensDS; i<(numSensDS + numSensDHT22); i++) {
    Serial.print(F("T"));
    Serial.print(i+1);
    Serial.print(F(":"));
    Serial.print(cTemperatura[i], 2);
    
    if (i == OKOLICA_0) {
      Serial.print(F("("));
      addrTmp = (addrLastHourTemp + numSensDHT22*120) + (unsigned int) minute() * sizeof(u2);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);

//      Serial.print(cTemperatura[i] - ((u2.uival/100.0)-50.0), 2);
      Serial.print(cTemperatura[i] - PretvotiEETemp2Float(u2.uival), 2);

      
      if (abs(u2.uival - cTemperatura[i]*100.0) > 1) {
//        u2.uival = (unsigned int) ((cTemperatura[i]+0.005+50.0)*100.0);
        u2.uival = PretvoriFloat2EETemp(cTemperatura[i]);

        delay(2);
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      delay(2);   
      Serial.print(F(")"));       
    }   
    
    Serial.print(F("/"));
    Serial.print(cVlaznost[i-numSensDS]);
    
    
    Serial.print(F("("));
    addrTmp = (addrLastHourTemp + 2*(numSensDHT22*120)) + (unsigned int) minute() * sizeof(u2);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);

      Serial.print(cVlaznost[i-numSensDS] - u2.uival/100.0, 2);
      
      if (abs(u2.uival - cVlaznost[i-numSensDS]*100.0) > 1) {
        u2.uival = (unsigned int) ((cVlaznost[i-numSensDS]+0.005)*100.0);

        delay(2);
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
      delay(2);   
      Serial.print(F(")"));       
      
    
    Serial.print(F("/"));
    Serial.print(cTempRosicsa[i-numSensDS]);
    Serial.print(F("/"));
    Serial.print(cHumidex[i-numSensDS]);
    Serial.print(F("-"));   
  }
  for (int i=numSensDS + numSensDHT22; i<(numSensDS + numSensDHT22 + numSensK); i++) {
    Serial.print(F("T"));
    Serial.print(i+1);
    Serial.print(F(":"));
    Serial.print(cTemperatura[i], 2);
    
    
    
    
    /*
    Serial.print(F("/"));
    Serial.print(cTemperatura[i] - cTemperatura[OKOLICA_0], 2);
    static float povpr_razlika =0.0;
    static unsigned long numM = 0;
    float sTemp = 0;
    for (int j=0; j < numSensDS + numSensDHT22; j++) {
      sTemp += cTemperatura[j];
    }
    sTemp /= (numSensDS + numSensDHT22);  
    povpr_razlika += (cTemperatura[i] - sTemp);
    numM += 1;
    Serial.print(F("/"));
    Serial.print(povpr_razlika/numM, 3);
  */  Serial.print(F("-"));   
  
  }
}



//--------------------------------------------------------------------------------
time_t processSyncMessage() {
  // return the time if a valid sync message is received on the serial port.
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      return (pctime); 
    }  
  }
  return 0;
}


//--------------------------------------------------------------------------------
float AutoTimeUnitConv(unsigned long cas, char *cunits)
{
  float ccas;
  
  if (cas > 3600) {
    ccas = cas/3600.0;
    sprintf(cunits, "h");
  }
  else if (cas > 60) {
    ccas = cas/60.0;
    sprintf(cunits, "m");
  }
  else {
    ccas = (float) cas;  
    sprintf(cunits, "s");
  }
  delay(1);
  return(ccas);
}

//-------------------------------------------------------------------------------
// Ali je trenutno obdobje vzdrzevanja nizje (nocne) temperature
boolean IsNTempCas()
{
  if (hour() >= uraVTemp[0] && hour() < uraVTemp[1])
    return(false);
  return(true); 
  /*
  if (hour() >= uraVTemp[1] || hour() < uraVTemp[0])
    return(true);
  return(false);   
  */
}


//-------------------------------------------------------------------------------
// Ali je trenutno obdobje vzdrzevanja nizje (nocne) temperature
boolean IsCasTransfTopl()
{
  if (hour() >= 22 || hour() < 4)
    return(true);
  return(false); 

}





//-------------------------------------------------------------------------------
// temporarely function



/*
// temporarely function
void last24H_Info(void)
{
  int i;
  int index24;
  int sens;
  
  for (sens = 0; sens < MAXSENSORS; sens++) {
    sum24[sens]-=temp24[sens][chour];
    temp24[sens][chour] = cTemperatura[sens];
    temp24Time[sens][chour] = now();
    sum24[sens]+=temp24[sens][chour];
  
    if (temp24[sens][chour] > maxTemp24[sens]) {
      maxTemp24[sens] = temp24[sens][chour];
      maxTemp24Time[sens] = temp24Time[sens][chour]; 
    }
    else if ((now() - maxTemp24Time[sens]) >= 24*60*60) {
      maxTemp24[sens] = temp24[sens][chour];
      index24 = chour;
      for (i=0; i<24; i++) {
        if (temp24[sens][i] > maxTemp24[sens]) {
          if (temp24[sens][i] > 0) {
            maxTemp24[sens] = temp24[sens][i];
            index24 = i;    
          }
        }
      }
//    maxTemp24[sens] = temp24[sens][index24];
      maxTemp24Time[sens] = temp24Time[sens][index24];
    }
  
    if (temp24[sens][chour] < minTemp24[sens]) {
      minTemp24[sens] = temp24[sens][chour];
      minTemp24Time[sens] = temp24Time[sens][chour]; 
    }
    else if ((now() - minTemp24Time[sens]) >= 24*60*60) {
      minTemp24[sens] = temp24[sens][chour];
      index24 = chour;
      for (i=0; i<24; i++) {
        if (temp24[sens][i] < minTemp24[sens]) {
          if (temp24[sens][i] > 0) {
            minTemp24[sens] = temp24[sens][i];
            index24 = i;    
          }
        }
      }
//      minTemp24[sens] = temp24[sens][index24];
      minTemp24Time[sens] = temp24Time[sens][index24];
    }
  }
  if (chour < 23)
    chour++;
  else
    chour = 0;  
}*/


//--------------------------------------------------------------------------------------------
unsigned int ObsegZgodovine(int sensor, unsigned int pred)
{
  unsigned int addrTmp;
  
  addrTmp = elapsedSecsToday(now())/(zapisXMin*60);  //naslov glede na zacetek dneva  
  if (zapisXMin*histLen > 1440) {                    //ce je zgodovina daljsa od enega dneva
    addrTmp += ((weekday()-NED)*(1440/zapisXMin));     //dodam naslovi se pretekle dneve v tednu 
  }
  if (pred > 0) {
    if (addrTmp >= (unsigned int)pred) {
      addrTmp = addrTmp - pred;
    }
    else {
      addrTmp = histLen+addrTmp-pred;
    }
  }  
  
  addrTmp += (sensor * histLen);    
  addrTmp *= sizeof(u2);
  addrTmp += addrTempBack;
  return (addrTmp);
}  




//-------------------------
float sum24[MAXSENSORS];
//float temp24[MAXSENSORS][24];
//unsigned long temp24Time[MAXSENSORS][24];
float maxTemp24[MAXSENSORS];
float minTemp24[MAXSENSORS];
unsigned long maxTemp24Time[MAXSENSORS];
unsigned long minTemp24Time[MAXSENSORS];

//-------------------------------------------------------------------------------
void last24H_Info(void)
{
  int i;
  int index24;
  int sens;
  unsigned int addrTmp;
  
  if (hour() == 0) {
    for (i=0; i<numSensDS; i++) {
      sum24[i] = 0.0;  
    }  
  }  
  
  for (sens = 0; sens < numSensDS; sens++) {
    if (sum24[sens] == 0) {

      maxTemp24Time[sens] = now()- 25UL*60UL*60UL;
      minTemp24Time[sens] = now()- 25UL*60UL*60UL;
      maxTemp24[sens] = cTemperatura[sens];
      minTemp24[sens] = cTemperatura[sens];
      Serial.print(F("T"));
      Serial.print(sens+1);
      Serial.print(F("-")); 
      for (i=24; i>0; i--) {  
        addrTmp = ObsegZgodovine(sens, i);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        delay(2);
        
        Serial.print(i);
        Serial.print(F("["));
        Serial.print(addrTmp);
        Serial.print(F("]"));
        Serial.print(F(":"));
        Serial.print(u2.uival/100.0);
        Serial.print(F(" "));
        sum24[sens] += u2.uival/100.0;
      }
      Serial.println(F(""));  
    }  
    else {
      if (sens == 0)
        Serial.println("");  
    }  
    
    delay(2);
    addrTmp = ObsegZgodovine(sens, 24);
    delay(2);
    i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
    delay(2);
    sum24[sens] -= ((float)u2.uival/100.0);
    Serial.print(F(" Sen:"));
    Serial.print(sens+1);
    Serial.print(F("Ad:"));
    Serial.print(addrTmp);
    Serial.print(F("T:"));
    Serial.print(u2.uival/100.0);
    
    sum24[sens] += cTemperatura[sens];
  
    if (cTemperatura[sens] > maxTemp24[sens]) {
      maxTemp24[sens] = cTemperatura[sens];
      maxTemp24Time[sens] = now(); 
    }
    if ((now() - maxTemp24Time[sens]) >= 86370UL) {
      addrTmp = ObsegZgodovine(sens, 23);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);
      maxTemp24[sens] = u2.uival/100.0;
      index24 = 23;
      for (i=22; i>=0; i--) {  
        addrTmp = ObsegZgodovine(sens, i);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        delay(2);
        if (u2.uival/100.0 > maxTemp24[sens]) {
          if (u2.uival > 0) {
            maxTemp24[sens] = u2.uival/100.0;
            index24 = i;    
          }
        }
      }
//    maxTemp24[sens] = temp24[sens][index24];
      maxTemp24Time[sens] = now()- (unsigned long) index24 * (unsigned long) (zapisXMin * 60UL);
    }
  
    if (cTemperatura[sens] < minTemp24[sens]) {
      minTemp24[sens] = cTemperatura[sens];
      minTemp24Time[sens] = now(); 
    }
    if ((now() - minTemp24Time[sens]) >= 86370UL) {
      addrTmp = ObsegZgodovine(sens, 23);
      i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      delay(2);
      minTemp24[sens] = u2.uival/100.0;
      index24 = 23;
      for (i=22; i>=0; i--) {  
        addrTmp = ObsegZgodovine(sens, i);
        i2c_eeprom_read_buffer(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
        delay(2);
        if (u2.uival/100.0 < minTemp24[sens]) {
          if (u2.uival > 0) {
            minTemp24[sens] = u2.uival/100.0;
            index24 = i;    
          }
        }
      }
//      minTemp24[sens] = temp24[sens][index24];
      minTemp24Time[sens] = now()- (unsigned long) index24 * (unsigned long) (zapisXMin * 60UL);
    }
  }
}




//--------------------------------------------------------------------------------
//
float RefTemp(byte refTempType)
{
 // return(AvgAllTimeTemp(CRPALKA_0));
  if (refTempType == B01) {
    return(avgTempVodeTC);
  }  
  return(cTemperatura[CRPALKA_0]);
}



#endif 

