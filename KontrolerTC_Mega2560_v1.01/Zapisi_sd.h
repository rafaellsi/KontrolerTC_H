#ifndef Zapisi_sd_h
#define Zapisi_sd_h


//extern int coRawVal;

void SDInit(void);
void ImeDatoteke(char *ime);
void ImeDatotekeOnOff(char* ime);
File OdpriDatoteko(char* imeDat, byte typeDat);
void PrintTempAllSDbin(void);
void ZapisiOnOffSD(int state, byte tipSpremembe);
void IzpisDataOnOffSerial(void);
void IzpisDatnaSerial(void);

// SD spremenljivke
Sd2Card cardSD;



//--------------------------------------------------------------------------------
void SDInit(void) {  
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
}

//--------------------------------------------------------------------------------
void ImeDatoteke(char* ime)
{
    
    sprintf(ime, "%04d%02d%02d.dat", year(), month(), day());
    
}

//--------------------------------------------------------------------------------
void ImeDatotekeOnOff(char* ime)
{
    sprintf(ime, "%02d%02d%02d.dat", year()-2000, month(), day());
}

//--------------------------------------------------------------------------------
File OdpriDatoteko(char* imeDat, byte typeDat)
{
  File myFile;
  
  myFile = SD.open(imeDat, typeDat); 
 
 if (!myFile) {
    sprintf(infoErr," ErrF01");
    Serial.print(infoErr);
    Serial.print(F(" "));
    Serial.print(imeDat);
    Serial.print(F(" "));
    delay(5);
    myFile = SD.open(imeDat, typeDat);
    delay(2);
    if (!myFile) {
      sprintf(infoErr," ErF01a");
      Serial.print(infoErr);
      delay(5);
      SDInit();
      delay(5);
      myFile = SD.open(imeDat, typeDat);
      delay(2);
      if (!myFile) {
        sprintf(infoErr," ErF01b");
        Serial.print(infoErr);
      }
    }
  }
  return(myFile);
}

//--------------------------------------------------------------------------------
void PrintTempAllSDbin(void)
{
  File myFile;
  char ime[13];
  
  ImeDatoteke(ime);
  
  
//  myFile = SD.open(ime, FILE_WRITE);
  myFile = OdpriDatoteko(ime, FILE_WRITE);
  /*
  if (!myFile) {
    sprintf(infoErr," ErrF01");
    Serial.print(infoErr);
    delay(5);
    myFile = SD.open(ime, FILE_WRITE);
    if (!myFile) {
      sprintf(infoErr," ErF01a");
      Serial.print(infoErr);
      SDInit();
      myFile = SD.open(ime, FILE_WRITE);
      if (!myFile) {
        sprintf(infoErr," ErF01b");
        Serial.print(infoErr);
      }
    }
  }
  */
  if (myFile) {
    NarediTimeStr(ime, now());
    myFile.print(ime);

    myFile.print(F(" "));
    myFile.print(now());
    
    for (int i=0; i<numSens; i++) { 
      myFile.print(F(" "));
      myFile.print(cTemperatura[i]);
      
    }
    for (int i=numSensDS; i<numSensDS+numSensDHT22; i++) {
      myFile.print(F("C  "));
      myFile.print(cVlaznost[i-numSensDS]);
      myFile.print(F("%"));
    }  
    myFile.print(F(" "));
    /*
    if (stateTC == TC_ON) {
      if (prevTCState == 1) 
        myFile.print(F("ON"));
       else
         myFile.print(F("SB"));
     }
     else
       myFile.print(F("OFF"));
    */
    if (releState_ventKompTC == R_TC_VENT_ON)
      myFile.print(F("V"));
  
    if (stateTC == STATE_TC_ON) {
      if (releState_kompTC == R_TC_KOMP_ON)
        myFile.print(F("K"));  
      if (releState_egrelecTC == R_TC_EGREL_ON)
        myFile.print(F("EG"));
    }
    else {
      myFile.print(F("OFF"));
    }
    
    myFile.print(F(" "));
    myFile.print(AC_mimax(), 3); 
    
    myFile.print(F("A CO:"));
    myFile.print(coRawValRef);
    myFile.print(F(" ("));
    myFile.print(AvgValULUL_F(coRawValSum, numMerCO), 3);
    myFile.print(F("/"));
    myFile.print(coRawValMax);
    myFile.print(F(")"));

    
    
    myFile.print(F(" I(12V):"));
    myFile.print(Tok_12V());
    myFile.print(F("("));
    myFile.print(AvgValFF_F(sumTok_12V, (float) nMerTok_12V), 4);
 //   myFile.print(sumTok_12V/((float) nMerTok_12V), 3);
    myFile.print(F("/"));
    myFile.print(maxTok_12V);
    myFile.print(F(")A"));
/*    
    if (digitalRead(CO_DOUT_PIN) == HIGH) 
      myFile.print(F(" CO alarm"));
    myFile.println(F(""));
*/      
    myFile.println(F(""));
    myFile.close();
    

  } 
  delay(100);  
}

//-------------------------------------------------------------------------
void ZapisiOnOffSD(int state, byte tipSpremembe = 0)
{
  // tipSpremembe:
  // * 0 - vklop/izklop TC
  // * 1 - vklop/izklop ventila TC
  // * 2 - vklop/izklop crpalke TC
  // * 10 - vklop/izklop crpalke rediatorjev
  // * 20 - vklop/izklop ventilatorja kompresorja TC
  // * 25 - vklop/izklop kompresorja TC
  // * 100 - hitrost gretja
  
  // state:
  // * 0 - izklop naprave
  // * 1 - vklop naprave
  // * 10 - info
  File myFile;
  
  char ime[13];
  ImeDatotekeOnOff(ime);
  delay(10);
  
//  myFile = SD.open(ime, FILE_WRITE);
  myFile = OdpriDatoteko(ime, FILE_WRITE);

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
        myFile.print(F("Vklop TC: "));
        NarediTimeStr(ime, lastTCStateChg);
        myFile.print(ime);

    
        myFile.print(F("  "));
        myFile.print(startTemp);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[OKOLICA_0]);
        myFile.println(F(")"));
      } //state == 1
      else if (state == 0) {
        myFile.print(F("Izklop TC: "));
        NarediTimeStr(ime, now());
        myFile.print(ime);
        myFile.print(F("  "));
        
        myFile.print(RefTemp(0));
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
      myFile.print(tKompSt-30.0);
      myFile.print(F(", "));
      myFile.print(tKompSt-15.0);
      myFile.print(F(", "));
      myFile.print(tKompSt+10.0);
      myFile.print(F(")= "));
      myFile.print(KompenzZacTemp(tKompSt-30.0), 4);
      myFile.print(F(", "));
      myFile.print(KompenzZacTemp(tKompSt-15.0), 4);
      myFile.print(F(", "));
      myFile.print(KompenzZacTemp(tKompSt+10.0), 4);
      
      myFile.println("");
      
      myFile.print(F(" factWeightAvgTemp: "));
      myFile.print(factWeightAvgTemp, 4);
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
        else if(tipSpremembe == 20) 
          myFile.print(F("Vkl. vent.TC: ")); 
        else if(tipSpremembe == 25) 
          myFile.print(F("Vkl. kompr.TC: "));  
          
        NarediTimeStr(ime, now());
        myFile.print(ime);

        myFile.print(F("  "));
        myFile.print(cTemperatura[CRPALKA_0]);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[PEC_DV]);
        myFile.print(F(" - "));
        myFile.print(cTemperatura[PEC_TC_DV]);
        myFile.println(F(")"));  
      }      //state == 1
      else {
        if (tipSpremembe == 1)
          myFile.print(F("Zap. ventila: "));
        else if (tipSpremembe == 2)
          myFile.print(F("Izkl.crpalke TC: "));
        else if(tipSpremembe == 10) 
          myFile.print(F("Izkl. crpalke rad.: "));
        else if(tipSpremembe == 20) 
          myFile.print(F("Izkl. vent.TC: "));
        else if(tipSpremembe == 25) 
          myFile.print(F("Izkl. kompr.TC: "));      
          
        NarediTimeStr(ime, now());
        myFile.print(ime);
    
        myFile.print(F("  "));
        myFile.print(cTemperatura[CRPALKA_0]);
        myFile.print(F(" ("));
        myFile.print(cTemperatura[PEC_DV]);
        myFile.print(F(" - "));
        myFile.print(cTemperatura[PEC_TC_DV]);
        myFile.println(F(")"));  
      } //else
    }  //else (state = 1)
    myFile.close();
  }
  delay(100);
}

//------------
void IzpisDataOnOffSerial(void)
{
//  char ch;
  File myFile;
  
  char ime[13];
  
  ImeDatotekeOnOff(ime);
  Serial.println(F(""));
  Serial.println(ime);
  
  
//  myFile = SD.open(ime, FILE_READ);
  myFile = OdpriDatoteko(ime, FILE_READ);
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


//--------------------------------------------------------------------------------
void IzpisDatnaSerial(void)
{
 // char ch;
  File myFile; 
 
  char ime[13];
  
  ImeDatoteke(ime);
  Serial.println(F(""));
  Serial.println(ime);
  
//  myFile = SD.open(ime, FILE_READ);
  myFile = OdpriDatoteko(ime, FILE_READ);
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




#endif
