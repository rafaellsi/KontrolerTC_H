#ifndef Temperature_h
#define Temperatue_h



// 1-Wire spremneljivke
OneWire ds(ONE_WIRE_BUS);
DeviceAddress devAddress[MAXSENSORS];  ///

float cTemperatura[MAXSENSORS];

float sumTemp[MAXSENSORS];
//unsigned long  numMer[MAXSENSORS];
byte type_s[MAXSENSORS];
//int numMer=0;

int numSens=0;

unsigned long convWaitTime = 1000;
boolean temeratureIzmerjene=true;


//float cTemperatura_TC[60];

//--------------------------------------------------------------------------------
// branje senzorja
static float PreberiTemperaturo(int cSens, boolean zahtevajBranje)
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
static boolean PreberiTemperatureDS(boolean zahtevajBranje, boolean allSens = false)
{
  float cTemp;
  
  if (allSens && !zahtevajBranje) {
    cTemp = PreberiTemperaturo(MAXSENSORS, zahtevajBranje);  
  }
  else {
    for (int i=0; i<numSens; i++) {
      cTemp = PreberiTemperaturo(i, zahtevajBranje);
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
// function to print a device address
static void PrintAddress(DeviceAddress deviceAddress)
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


float sumCTemp=0.0;
float sumCTempEMA=0.0;
//--------------------------------------------------------------------------------
// function to print the temperature for a device
static void PrintTemperatureAll(void)
{
  unsigned int addrTmp;
 
  for (int i=0; i<numSens; i++) {
    
    Serial.print(F("T"));
    Serial.print(i+1);
    Serial.print(F(":"));
    Serial.print(cTemperatura[i], 2);
    
    if (i == crpalka_0) {
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

      Serial.print(cTemperatura[i] - u2.uival/100.0, 2);
      

/*      
      sumCTempEMA -= sumCTemp;
//      sumCTemp -= cTemperatura_TC[minute()];
      sumCTemp -= u2.uival/100.0;
//      cTemperatura_TC[minute()] =  cTemperatura[i];
      sumCTemp += cTemperatura[i];
      sumCTempEMA += (60.0*cTemperatura[i]);
*/      
      if (abs(u2.uival/100.0 - cTemperatura[i]) > 0.01) {
        u2.uival = cTemperatura[i]*100;
        delay(2);
        i2c_eeprom_write_page(AT24C32_I2C_ADDR, addrTmp, AT24C32_ADDR_LENGH, (byte *)&u2, sizeof(u2));
      }
/*     
      Serial.print("|");
      Serial.print((cTemperatura[i] - (sumCTemp/60.0))*2.0, 2);
      Serial.print("|");
      Serial.print((cTemperatura[i] - (sumCTempEMA/1830.0))*2.0, 2);
      Serial.print("|");
      Serial.print(((sumCTempEMA/1830.0) - (sumCTemp/60.0)), 2);
*/      Serial.print(F(")"));
        
    }    
//    sendPlotData(cTemperatura[i], 2);
    
    Serial.print(F("-"));
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
static float AutoTimeUnitConv(unsigned long cas, char *cunits)
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
static boolean IsNTempCas()
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
static boolean IsCasTransfTopl()
{
  if (hour() >= 22 || hour() < 4)
    return(true);
  return(false); 

}



//--------------------------------------------------------------------------------
//
static boolean IsWeekend()
{
  if (weekday() == NED || weekday() == SOB)
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
static unsigned int ObsegZgodovine(int sensor, unsigned int pred =0)
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




float sum24[MAXSENSORS];
//float temp24[MAXSENSORS][24];
//unsigned long temp24Time[MAXSENSORS][24];
float maxTemp24[MAXSENSORS];
float minTemp24[MAXSENSORS];
unsigned long maxTemp24Time[MAXSENSORS];
unsigned long minTemp24Time[MAXSENSORS];
//int chour=0;
//-------------------------------------------------------------------------------
void last24H_Info(void)
{
  int i;
  int index24;
  int sens;
  unsigned int addrTmp;
  
  if (hour() == 0) {
    for (i=0; i<numSens; i++) {
      sum24[i] = 0.0;  
    }  
  }  
  
  for (sens = 0; sens < numSens; sens++) {
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




#endif 

