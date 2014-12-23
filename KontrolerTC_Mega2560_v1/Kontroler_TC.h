
#ifndef Kontroler_TC_h
#define Kontroler_TC_h

/*
#include <inttypes.h>

*/

#include <Arduino.h>
#include <Wire.h> 

typedef union  {
    byte by[4];
    unsigned long ulval;
  } num2byte4b;

typedef union  {
    byte by[4];
    float fval;
  } num2byte4f;

typedef union  {
    byte by[2];
    unsigned int uival;
  } ui2byte2;

ui2byte2 u2;

//--------------------------------------------------------------------------------
static void NarediTimeStr(char* cas, unsigned long tcas, boolean izpSec = true)
{   
  if (izpSec)
    sprintf(cas, "%02d:%02d:%02d", hour(tcas), minute(tcas), second(tcas));
  else 
    sprintf(cas, "%02d:%02d", hour(tcas), minute(tcas));
}

//--------------------------------------------------------------------------------
static void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte addrlen, byte* up, byte length ) 
{   
  Wire.beginTransmission(deviceaddress);
  if (addrlen > 1) 
    Wire.write((int)(eeaddresspage >> 8)); // MSB
  Wire.write((int)(eeaddresspage)); // LSB
  int c;
  for (c=0; c < length; c++) { 
    Wire.write(up[c]);
    delay(1);
  }
  Wire.endTransmission();
}


 static void i2c_eeprom_write_byte(int deviceaddress,unsigned int eeaddress, byte addrlen, byte data) {
//    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1)
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((eeaddress)); // LSB
    Wire.write(data);
    Wire.endTransmission();
  }



static void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte addrlen, byte *up, int length ) {
    
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1) 
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress)); // LSB
    
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,length);
//    delay(1);
    int c = 0;
    for (c=0; c < length; c++) {  
      delay(1); 
      if (Wire.available()) {
         up[c] = Wire.read();
       }
    }
  }
  
  
static byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress, byte addrlen) {
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1)
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((eeaddress )); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    delay(1);
    if (Wire.available()) 
      rdata = Wire.read();
    return rdata;
}

static float Sec2Hour(unsigned long sec)
{
  return((float)sec/(float)SECS_PER_HOUR);
}

  
#endif 
