#ifndef Eprom_external_h
#define Eprom_external_h

//#include <Wire.h>  

void waitEEReady(int deviceAddress);

//--------------------------------------------------------------------------------
void i2c_eeprom_write_page( int deviceaddress, unsigned int eeaddresspage, byte addrlen, byte* up, byte length ) 
{   
//  waitEEReady(deviceaddress);
  
  Wire.beginTransmission(deviceaddress);
  if (addrlen > 1) 
    Wire.write((byte)(eeaddresspage >> 8)); // MSB
  Wire.write((byte)(eeaddresspage)); // LSB
//  delay(2);
  
//  if (Wire.write(up, length) != length)
//    Serial.print(F("ErrEprWrPgLe"));

  for (int c=0; c < length; c++) { 
    Wire.write(up[c]);
    delay(1);
  }
  

  byte success = Wire.endTransmission();
  if (success != 0) {
    Serial.print(F("ErrEpWrPg "));
    Serial.print(success);
  }
  waitEEReady(deviceaddress); 
  delay(1);
}

//--------------------------------------------------------------------------------
void i2c_eeprom_write_byte(int deviceaddress,unsigned int eeaddress, byte addrlen, byte data) {
//    int rdata = data;
    byte numW = 0;
    
//    waitEEReady(deviceaddress);
    
    Wire.beginTransmission(deviceaddress);

    if (addrlen > 1)
      numW += Wire.write((byte)(eeaddress >> 8)); // MSB
    numW += Wire.write((byte)(eeaddress)); // LSB

//    Wire.write((int) eeaddress);
    numW += Wire.write(&data, 1);
    
//  Wire.endTransmission(true);

  byte success = Wire.endTransmission();
  if (success != 0) {
    Serial.print(F("ErrEpWrBy "));
    Serial.print(success);
    Serial.print(F(" "));
    Serial.print(numW);
    Serial.print(F(" "));
    Serial.write(&data, 1);
  } 
  waitEEReady(deviceaddress);
  delay(1);
}


//--------------------------------------------------------------------------------
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte addrlen, byte *up, int length ) {
    byte success = 1;  
  
    waitEEReady(deviceaddress);
    delay(1);
    
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1) 
      Wire.write((byte)(eeaddress >> 8)); // MSB
    Wire.write((byte)(eeaddress)); // LSB
    
    
    success = Wire.endTransmission();
    
  
    byte retByts = Wire.requestFrom(deviceaddress,length);

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
    
    if (success != 0) {
      Serial.print(F("ErrEpReBu "));
      Serial.print(success);
    } 
    if (retByts != length)
      Serial.print(F("ErrEprReBuLe"));
      
//    waitEEReady(deviceaddress);
    
  }
  
//--------------------------------------------------------------------------------  
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress, byte addrlen) {
    byte rdata = 0xFF;
    byte success = 1;
    
    waitEEReady(deviceaddress);
    delay(1);
    
    Wire.beginTransmission(deviceaddress);
    if (addrlen > 1)
      Wire.write((byte)(eeaddress >> 8)); // MSB
    Wire.write((byte)(eeaddress )); // LSB
    
    success = Wire.endTransmission();
    
    
    
    Wire.requestFrom(deviceaddress,1);
    delay(1);
    if (Wire.available()) 
      rdata = Wire.read();
    
    if (success != 0) {
      Serial.print(F("ErrEpReBu "));
      Serial.print(success);
    } 
    
//    waitEEReady(deviceaddress);
    
    return rdata;
}


//-------------------------------------------------------------------------------- 
void waitEEReady(int deviceAddress)
{
unsigned long i2c_check_delay = 100;
    
    // Wait until EEPROM gives ACK again.
    // this is a bit faster than the hardcoded 5 millis
    int x = -1;
    unsigned long lastWrite = millis();
    
    while ((millis() - lastWrite) <= i2c_check_delay )
    {
        Wire.beginTransmission(deviceAddress);
        x = Wire.endTransmission();
        if (x == 0) break;
//        else Serial.print(F(","));
    }
}

#endif
