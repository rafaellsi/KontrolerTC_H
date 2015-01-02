#ifndef Eprom_external_h
#define Eprom_external_h

#include <Wire.h>  
//--------------------------------------------------------------------------------
static void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte addrlen, byte* up, byte length ) 
{   
  Wire.beginTransmission(deviceaddress);
  if (addrlen > 1) 
    Wire.write((int)(eeaddresspage >> 8)); // MSB
  Wire.write((int)(eeaddresspage)); // LSB

  for (int c=0; c < length; c++) { 
    Wire.write(up[c]);
    delay(1);
  }
  Wire.endTransmission(true);
}

//--------------------------------------------------------------------------------
 static void i2c_eeprom_write_byte(int deviceaddress,unsigned int eeaddress, byte addrlen, byte data) {
//    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1)
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((eeaddress)); // LSB
    Wire.write(data);
    Wire.endTransmission(true);
  }


//--------------------------------------------------------------------------------
static void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte addrlen, byte *up, int length ) {
    
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1) 
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress)); // LSB
    
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,length);
//    delay(1);
/*
    for (int c=0; c < length; c++) {  

      delay(1); 
      if (Wire.available()) {
         up[c] = Wire.read();
//         Serial.write(up[c]);
       }
    }
*/  int c = 0;
    while (Wire.available()) {
      up[c] = Wire.read();
      c++;
    }  
  }
  
//--------------------------------------------------------------------------------  
static byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress, byte addrlen) {
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1)
      Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((eeaddress )); // LSB
    Wire.endTransmission(true);
    Wire.requestFrom(deviceaddress,1);
    delay(1);
    if (Wire.available()) 
      rdata = Wire.read();
    return rdata;
}



#endif
