#ifndef Zapisi_sd_h
#define Zapisi_sd_h



// SD spremenljivke
Sd2Card cardSD;

File myFile;

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
static void ImeDatoteke(char* ime)
{
    
    sprintf(ime, "%04d%02d%02d.dat", year(), month(), day());
    
}

//--------------------------------------------------------------------------------
static void ImeDatotekeOnOff(char* ime)
{
    sprintf(ime, "%02d%02d%02d.dat", year()-2000, month(), day());
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
    for (int i=numSens; i<numSens+numSensDHT22; i++) {
      myFile.print(F("C  "));
      myFile.print(cVlaznost[i]);
      myFile.print(F("%"));
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
        myFile.print(cTemperatura[OKOLICA_0]);
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






#endif
