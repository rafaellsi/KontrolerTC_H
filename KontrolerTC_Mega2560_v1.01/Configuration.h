#ifndef Configuration_h
#define Configuration_h


extern void Beep(unsigned char);

//#define INTFACT
#define VERSION __DATE__ " " __TIME__ //"v1"
#define MAXSENSORS 9     //maksimalno stevilo temp sensorjev DS18x20
#define MAXSENSORS_DS 7
#define MAX_DHT22_SENS 1

//----------
#define NED  1    //nedelja, kot rezltat weekday()...
#define SOB  7    //sobota, .....

//----------

//#define USEDEVICEHUB
//#define USEDEVICEHUB_MQTT

//boolean useDeviceHub = false;
//boolean useDeviceHubMQTT = false;
boolean debugDeviceHub=false; 


#define USETHINGSPEAK
//boolean useThingspeak=true;

//---------

#define USEEGRELEC false    //ali se uporablja električni grelec

//#define R_TC_ON   LOW      // z RELE_TC - električni grelec ali Priključek TC
//#define R_TC_OFF  HIGH
#define STATE_TC_ON   1      // z RELE_TC - električni grelec ali Priključek TC
#define STATE_TC_OFF  0

//in/out state definition realys


#define R_CRAD_ON   LOW      // z RELE_CRAD
#define R_CRAD_OFF  HIGH     // rele crpalke radijatorjev

#define R_CTC_ON   LOW      //z RELE_CTC 
#define R_CTC_OFF  HIGH     //rele za crpalko na povezavi TC-pec

#define CEV_TERM_ON   LOW    //cevni termostat - internal pullup
#define CEV_TERM_OFF  HIGH

#define R_TC_EGREL_ON   LOW      // z RELE_TC_EGREL
#define R_TC_EGREL_OFF  HIGH     // električni grelec ali Priključek TC 

#define R_TC_VENT_ON  LOW  // z RELE_TC_VENT 
#define R_TC_VENT_OFF  HIGH  // ventilator kompresorja TC

#define R_TC_KOMP_ON  LOW  // z RELE_TC_KOMP 
#define R_TC_KOMP_OFF  HIGH  // kompresor TC


#define STATE_CRP_TC_ON   1
#define STATE_CRP_TC_OFF  0

#define STATE_VENT_TC_ON    1
#define STATE_VENT_TC_OFF   0
#define STATE_VENT_TC_NDEF 255
//----------
// pin and address definition

#define SENS_TOK_12V  A2    //pin za merjenje toka 12V - swithcher

#define T_KTYP_01_PIN A3   //pin k-type sensorja 1 
#define CO_SENS_APIN  A6    //vrednost CO senzorja
#define SENS_V5_3     A7    //pin za merjenje zun. napajanja 5V-2 za CO sensor- DC-DC
#define SENS_V12      A8    //pin za merjenje zun. napetosti napajanja 12V - switcher napajalnik
#define SENS_V5_2     A9    //pin za merjenje zun. napajanja 5V - DC-DC
#define SENS_RTC_BATT A10  //pin za merjenje napetsti RTC baterije - ni priklopljeno - ne dela RTC preko baterijae ...
#define SENS_TOK      A11   //senzor toka - 230v komplet
#define SENS_3V3_SD   A12   //napestost 3,3v na SD
//#define SENS_V_CO     A13   //napetost na CO sensorju



#define CO_DOUT_PIN  11
#define CO_PWR_PIN   12    

#define ENC_DT_PIN    2     // Encoder DT signal - interuppt 1
#define ENC_CLK_PIN   3     // Encoder CLK pin - interuppt 0
#define ENC_SW_PIN    4
// Encoder push button switch

#define ENC_PIN_A   ENC_DT_PIN
#define ENC_PIN_B   ENC_CLK_PIN

#define NRF24_CE   8
#define NRF24_CSN  9

#define ETHER_CS_PIN  10    //Ethrnet (ENC28J60) CS pin
#define ETHER_RESET_PIN 23

#define LCD_OSW_SW    7     //pin za vklop osvetlitve LCD-ja
#define LCD_RS    22        //LCD RS
//#define LCD_RW    23        //LCD RW
#define LCD_EN    24        //LCD EN
#define LCD_D4    25        //LCD D4
#define LCD_D5    26        //LCD D5
#define LCD_D6    27        //LCD D6
#define LCD_D7    28        //LCD D7

#define BEEP_PIN     29    //buzzer pin
#define ONE_WIRE_BUS 30    //1-wire na pin D4 - D30

#define DHTPIN 31
#define DHTTYPE DHT22

#define CEVTERM_PEC_TC  32    //cevni termostat na peci proti TC - vklopi internal pull-up
//#define TEMOSTAT_VEZA_I   16
//#define TEMOSTAT_VEZA_II  17

#define STIKALO_CRP_RAD_ON 33    //stikalo 1
#define STIKALO_CRP_RAD_OFF 34   //stikalo 1 
#define STIKALO_CRP_TC_ON 35     //stikalo 2 
#define STIKALO_CRP_TC_OFF 36
#define STIKALO_TC_ON 37         //stikalo 3
#define STIKALO_TC_OFF 38



//#define stikalo  37            //stikalo 4
//#define stikalo  38

#define RELE_TC_KOMP  39    // rele 4/4 kompresor TC  
#define RELE_TC_VENT  40    // rele 3/4 ventilator TC
#define RELE_CTC    41    // rele 2/4 za crpalko na povezavi TC-pec
#define RELE_CRAD   42    // rele 1/4 crpalke radijatorjev
//#define RELE_EL_GREL_TC     43    //rele 1 proklopljen na pin D7 -> D40 - vklop  TC
#define RELE_TC_EGREL     43    // RELE_TC - električni grelec ali Priključek TC

//#define L298 En2  44    //ENB  - enable input 3,4
//#define L298 1/2  45    //l298 4/4  - input 3
//#define L298 2/2  46    //l298 3/4  - input 3
#define VENTTC_1    47    // na L298  - input 1
#define VENTTC_2    48    // na L293  - input 2
#define VENTTC_EN   49    // ENA na L298 - enable input 1,2  

#define SD_CS_PIN  53     //SD card CS+ pin 





//----------
#define DS1307_CTRL_ID 0x68  //i2C/TWI - naslov RTC cipa
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message


//----------
#define AT24C32_I2C_ADDR 0x50  //i2C/TWI - naslov memory cipa na RTC modulu
#define AT24C32_MAX_ADDR 4096  // velikost mamory cipa na RTC modulu
#define AT24C32_ADDR_LENGH 2   // dolžina nalova na memory cipu RTC modula

//----------
// temperature sensors indexes
/*
int   crpalka_0 = 1;         // index sensorja na crpalki
int   okolica_0 = 0;         // index enzorja okolice
int   pec_pv = 2;   // pec, povratni vod
int   pec_dv = 3;   // pecc, dvizni vod
int   rad_pv = 4;   // radijatorji, povratni vod
int   rad_dv = 5;   // radijatorji, dvizni vod
int   pec_TC_dv = 6; // dvizni vod proti TC
*/

int numSensDS = 0;        //število DS senzorjev
int numSensDHT22 = 1;
int numSensK = 1;
int numSens;

float kTypeOffset = 0.0; //3.67;

#define  OKOLICA_0  7   // index senzorja okolica
#define  CRPALKA_0  1   // index sensorja na TC
#define  PEC_PV     2   // pec, povratni vod
#define  PEC_DV     3   // pecc, dvizni vod
#define  RAD_PV     4   // radijatorji, povratni vod
#define  RAD_DV     5   // radijatorji, dvizni vod
#define  PEC_TC_DV  6   // dvizni vod proti TC

char sensorIme[MAXSENSORS][6] = {"Okol", "ToCrp", "PecPV", "PecDV", "RadPV", "RadDV", "PecTC", "Okol", "Dimn"};

typedef union  {
    byte by[4];
    unsigned long ulval;
  } ul2byte4b;

typedef union  {
    byte by[4];
    float fval;
  } fl2byte4b;

typedef union  {
    byte by[2];
    unsigned int uival;
  } ui2byte2;

ui2byte2 u2;






//------------------------------------------
//boolean measureOnly = false;
boolean showCRC = true;  // izpis kontrolnih parametrov na serial

//------------------------------------------
unsigned int   merXMin = 1;          //temperaaturo merimo vsakih merXMin minut
float minTempVTOn = 37.5;    //min temp- vode za vklop pri visoki tarifi t < 
int   minRunTimeMin = 10;    // minimalni cas (delovanja)  ali ne-delovanja TC / (odvisno od UpostevajElTarife(void)) - ne vec

//------------------------------------------
float dTemp = 5.0;  // default 5.0
//float sensDiff = 2.5;

//------------------------------------------


//------------------------------------------
float minTempNightOn = 32.5;
float ciljnaTemp = 52.0;
float ciljnaTempWeekend = 48.0;
//float deltaTh = 5.55;   // do 16.Oct.2012
//float deltaTh = 4.35;   //
//float tempPregrvanja = 60.0;
float ciljnaTempPregrevanja = 60.0;
unsigned long intervalPregrevanja_Sec = 1425600UL;  // (16dni*24h + 12h)*60min*60s

int statePregrevanje = 0;

float min_TempOK_TCKomp = 5.0;
float max_TempOK_TCKomp = 35.0;

float maxDovTempVodeTC_Komp = 68.0;

float deltaTh;
float deltaThOk;
float deltaThSt;

float refTempIzrac = -1.0;


const float tKompOK = 20.0;
//const float tKompSt = 20.0;
const float tKompSt = 60.0;

int zamikMerTemp = 15;  // hitrost gretja se računa "zamikMerTemp" po izklopu TC
                        // povprečenje temperature vode v TC za izračun izklopa TC

//------------------------------------------
int uraVTemp[] = {6, 21};  //obdobje vklopa pri temp minTempVTOn - 0-start, 1-end
//int vStartUraOff = 20;  //ura zacetka znizevanja temerature za izklop
int dolPrehObd = 121;     //dolzina prehodnega obdobja postopnega znizevanja temp. za izklop (v minutah)
int startUraVT = 6;       // zacetek visoke tarife
int startUraNT = 22;      // zacetek nizke tarife :)
float mejaToka = 1.0;     // meja velikosti toka, da se smatra da kompresor teče 

//------------------------------------------
// boolean upostevajElTarife = false; // glej opombo pod verzijo v0.45 && v0.54
//povezava crpalka - pec 
// -- spremenjeno 23.12.2015
float tempVklopaCrpTC = 75.0 ; //best = 75.0;       // temp. vklopa crpalke TC
//float tempVklopaCrpTC = 82.5 ; //best = 75.0;       // temp. vklopa crpalke TC
float histCrpTC = 4.95;          // min. diff. med vkolopm in izklopom crpalke TC
//float tempIzklopaCrpTC = 70.0;      //
float minDiffTCPec = 2.0;           // min. temp. diferenca mad pecjo TC (pec > TC)
float tempIzklopaVentCrpTC = 60.0;  // best 60.0// temp. nad katero ventil TC ostane odprt
float minTempPecPonovnoOdpVent = 55.0; //min. temp peci za ponovno odpiranje ventila - z pogojem temp. TC

//------------------------------------------
int minCrpTCRunTimeMin = 2;
int zaksnitevCrpVent_Sec = 20;
int minCrpRadRunTimeMin = 7;

//------------------------------------------
byte prevCrpRadState = 0;
float minDiffDvOkolCrpRad = 10.0;


//------------------------------------------
//float limTempCrpRad[]
//float limTempFactCrpRad[24];
float minMejnaTempRel  = 0.399;

float maxTempPVRad = 50.0; // max temperatura povratnega voda
float maxTempDVPec = 90.01; // max. temperatura dviznega voda peci
float maxDeltaDev = 5.0;

boolean isCrpRadAsAbsTemp = true; //upobi absol. temp za zagon crpalke


static float vccInternal = 5.0;


//------------------------------------------

//------------------------------------------
unsigned int addrLastChg = 0;
unsigned int addrPrevTCState = 4;
unsigned int addrOnTime = 5;
unsigned int addrDeltaTh = 9;
unsigned int addrDeltaThOk = 13;
unsigned int addrDeltaThSt = 17;

unsigned int addrFactWeightAvgTemp = 21;
unsigned int addrLastPregrevTime = 25;

unsigned int addrLastHourTemp = 40; // 128
unsigned int addrTempBack = 400;     //968
//-----------------------------------------
unsigned int histLen = 168;
int zapisXMin = 60;

//--------------------------------------
float cTemperatura[MAXSENSORS];
byte type_s[MAXSENSORS];          //tip temp sensorja 
float sumTemp[MAXSENSORS + MAX_DHT22_SENS];

float cVlaznost[MAX_DHT22_SENS];
float cHumidex[MAX_DHT22_SENS];
float cTempRosicsa[MAX_DHT22_SENS];


unsigned long onTimeTC = 0;
byte prevTCState;
unsigned long lastTCStateChg = 0;  //cas zadnjega preklopa TC ali kompresorja TC
unsigned long lastPregrevTime = 0;

byte releState_ventKompTC = R_TC_VENT_OFF;
byte releState_kompTC = R_TC_KOMP_OFF;
byte releState_egrelecTC = R_TC_EGREL_OFF;
byte  stateTC = STATE_TC_OFF; 
    // stateTC = B00000001 - ventilator ON
    //         = B00000010 - kompresro ON
    //         = B00000100 - el. grelec ON
//boolean releState_TC = R_TC_OFF;  // stanje releja vklopa TC

unsigned long lastCrpTCStateChg;
unsigned long onTimeCrpTC=0;

unsigned long lastCrpRadStateChg;
unsigned long onTimeCrpRad=0;

unsigned long casMeritve;        //cas meritve parametrov
unsigned long prevCasMeritve;    //cas prejšnje meritve parametrov

float hitrGret = 0;
float startTemp;

float lastDeltaTh;
float lastDeltaThOk;
float lastDeltaThSt;

float lastHourTempChange;

unsigned long lastRunTime;
float tempOkolicaSt;

float porabaWH = 0.0;
float zacPorabaWH;
float Qv;
float We;

float Vrms = 230.0;

byte manuCrpTCState = 0;

int coRawVal;
int osvetlitevLCD = 0;

float sumTok_12V = 0.0;
float maxTok_12V;
unsigned long nMerTok_12V = 0;




char infoErr[8];

ul2byte4b u4;
fl2byte4b uf;

typedef uint8_t DeviceAddress[8];
//definicija naslovov senzorjev


//LiquidCrystal lcdA(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
LiquidCrystal lcdA(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
//DHT22 myDHT22(DHT22_PIN);
DHT dht(DHTPIN, DHTTYPE);
// 1-Wire spremneljivke
OneWire ds(ONE_WIRE_BUS);

void FiksAdrrSens(DeviceAddress devAddress[], byte *type_s)
{

//  normal
 
// okolica Na DS1307
  
  
  devAddress[0][0] = 0x28;
  devAddress[0][1] = 0xAF;
  devAddress[0][2] = 0xD9;
  devAddress[0][3] = 0x9E;
  devAddress[0][4] = 0x04;
  devAddress[0][5] = 0x00;
  devAddress[0][6] = 0x00;
  devAddress[0][7] = 0xA1;

//  type_s[0] = 0;
  numSensDS++;
  
  // crpalka
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0xA4;
  devAddress[1][2] = 0x44;
  devAddress[1][3] = 0x9F;
  devAddress[1][4] = 0x04;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0x8D;
  
  /*
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0xC2;
  devAddress[1][2] = 0x3A;
  devAddress[1][3] = 0x97;
  devAddress[1][4] = 0x03;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0x55;
  */
 /* 
  /devAddress[1][0] = 0x28;
  devAddress[1][1] = 0x32;
  devAddress[1][2] = 0x5B;
  devAddress[1][3] = 0x97;
  devAddress[1][4] = 0x03;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0xFB;
//  type_s[1] = 0;
*/  
  numSensDS++;
  
  
  //pec - povratni - prej na cevi TC
  devAddress[2][0] = 0x28;
  devAddress[2][1] = 0x99;
  devAddress[2][2] = 0x73;
  devAddress[2][3] = 0x97;
  devAddress[2][4] = 0x03;
  devAddress[2][5] = 0x00;
  devAddress[2][6] = 0x00;
  devAddress[2][7] = 0x99;
//  type_s[2] = 0;
  numSensDS++;
  
 
  
  
   // pec dvizni vod
  devAddress[3][0] = 0x28;
  devAddress[3][1] = 0x32;
  devAddress[3][2] = 0x7C;
  devAddress[3][3] = 0x97;
  devAddress[3][4] = 0x03;
  devAddress[3][5] = 0x00;
  devAddress[3][6] = 0x00;
  devAddress[3][7] = 0x52;
//  type_s[3] = 0;
   numSensDS++;
   
   // povratni vod radiatorji
  devAddress[4][0] = 0x28;
  devAddress[4][1] = 0xEA;
  devAddress[4][2] = 0x3A;
  devAddress[4][3] = 0x97;
  devAddress[4][4] = 0x03;
  devAddress[4][5] = 0x00;
  devAddress[4][6] = 0x00;
  devAddress[4][7] = 0x42;
//  type_s[4] = 0;
   numSensDS++;
   
   // dvizni vod radiatorji
  devAddress[5][0] = 0x28;
  devAddress[5][1] = 0x02;
  devAddress[5][2] = 0x8B;
  devAddress[5][3] = 0x97;
  devAddress[5][4] = 0x03;
  devAddress[5][5] = 0x00;
  devAddress[5][6] = 0x00;
  devAddress[5][7] = 0x48;
//  type_s[5] = 0;
  numSensDS++;
  
  devAddress[6][0] = 0x28;
  devAddress[6][1] = 0xD7;
  devAddress[6][2] = 0x85;
  devAddress[6][3] = 0x97;
  devAddress[6][4] = 0x03;
  devAddress[6][5] = 0x00;
  devAddress[6][6] = 0x00;
  devAddress[6][7] = 0xC5;
//  type_s[6] = 0;
  numSensDS++;
/* 
  
  //  test
  // okolica
  devAddress[0][0] = 0x28;
  devAddress[0][1] = 0xC4;
  devAddress[0][2] = 0xE3;
  devAddress[0][3] = 0x9E;
  devAddress[0][4] = 0x04;
  devAddress[0][5] = 0x00;
  devAddress[0][6] = 0x00;
  devAddress[0][7] = 0xA3;
//  type_s[0] = 0;
  numSensDS++;

// okolica Na DS1307
  devAddress[3][0] = 0x28;
  devAddress[3][1] = 0xC2;
  devAddress[3][2] = 0x3A;
  devAddress[3][3] = 0x97;
  devAddress[3][4] = 0x03;
  devAddress[3][5] = 0x00;
  devAddress[3][6] = 0x00;
  devAddress[3][7] = 0x55;
//  type_s[0] = 0;
  numSensDS++;
 
  // crpalka
/  devAddress[1][0] = 0x28;
/  devAddress[1][1] = 0x32;
/  devAddress[1][2] = 0x5B;
/  devAddress[1][3] = 0x97;
/  devAddress[1][4] = 0x03;
/  devAddress[1][5] = 0x00;
/  devAddress[1][6] = 0x00;
/  devAddress[1][7] = 0xFB;
/  type_s[1] = 0;
  
  
  //pec - povratni - prej na cevi TC
  devAddress[2][0] = 0x28;
  devAddress[2][1] = 0xA4;
  devAddress[2][2] = 0x44;
  devAddress[2][3] = 0x9F;
  devAddress[2][4] = 0x04;
  devAddress[2][5] = 0x00;
  devAddress[2][6] = 0x00;
  devAddress[2][7] = 0x8D;
//  type_s[2] = 0;
  numSensDS++;
  
 
  
  
   // pec dvizni vod
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0xAF;
  devAddress[1][2] = 0xD9;
  devAddress[1][3] = 0x9E;
  devAddress[1][4] = 0x04;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0xA1;
//  type_s[3] = 0;
  numSensDS++;
   // povratni vod radiatorji
  devAddress[4][0] = 0x28;
  devAddress[4][1] = 0x31;
  devAddress[4][2] = 0x38;
  devAddress[4][3] = 0x9F;
  devAddress[4][4] = 0x04;
  devAddress[4][5] = 0x00;
  devAddress[4][6] = 0x00;
  devAddress[4][7] = 0x9B;
//  type_s[4] = 0;
  numSensDS++;
   // dvizni vod radiatorji
  devAddress[5][0] = 0x28;
  devAddress[5][1] = 0xB3;
  devAddress[5][2] = 0xD7;
  devAddress[5][3] = 0x9E;
  devAddress[5][4] = 0x04;
  devAddress[5][5] = 0x00;
  devAddress[5][6] = 0x00;
  devAddress[5][7] = 0x25;
//  type_s[5] = 0;
  numSensDS++;
  
  devAddress[6][0] = 0x28;
  devAddress[6][1] = 0x67;
  devAddress[6][2] = 0xA1;
  devAddress[6][3] = 0x9E;
  devAddress[6][4] = 0x04;
  devAddress[6][5] = 0x00;
  devAddress[6][6] = 0x00;
  devAddress[6][7] = 0xCC;
//  type_s[6] = 0;
  numSensDS++;
*/

  if (numSensDS > MAXSENSORS_DS) {
    numSensDS = MAXSENSORS_DS;
    Serial.print(F("Stevilo DS senosrjev!!"));
    lcdA.clear();
    lcdA.print(F("Stevilo DS senosrjev!!"));
    Beep(250);
    
   }   
 
  for (int i=0; i<numSensDS; i++) {
    Serial.print(F("Temp. sensor "));
    Serial.print(i);
    switch (devAddress[6][0]) {
      case 0x10:
        Serial.println(F(": DS18S20"));  // or old DS1820
        type_s[i] = 1;
        break;
      case 0x28:
        Serial.println(F(": DS18B20"));
        type_s[i] = 0;
        break;
      case 0x22:
        Serial.println(F(": DS1822"));
        type_s[i] = 0;
        break;
      default:
        Serial.println(F(": ni DS18x20 family device."));
        return;
    }
  }   
  
//  return (7);
} 

//PROGMEM_getAnything (&limTempCrpRad[i]) 
const int limTempCrpRad[24] PROGMEM = {30, 30, 30, 30, 30, 30, 
                         28, 15, 12, 13, 15, 17, 
                         19, 21, 20, 18, 15, 12, 
                         14, 16, 18, 22, 30, 30};
/*
//pred 7.12.2014 float
float limTempCrpRad[24] = {30, 30, 30, 30, 30, 30, 
                         28, 15, 12, 13, 15, 17, 
                         19, 21, 20, 18, 15, 12, 
                         14, 16, 18, 22, 30, 30};

*/
//{30, 30, 30, 30, 30, 30, 28, 15, 12, 13, 15, 17, 19, 21, 20, 18, 15, 12, 14, 16, 18, 22, 30, 30};

const float limTempFactCrpRad[24] PROGMEM = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 
                             0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 
                             0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 
                             0.2, 0.2, 0.2, 0.2, 0.2, 0.2};

/*
float crpRadAsAbsTemp[24] = {15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 
                             17.0, 19.0, 22.0, 25.0, 30.0, 30.0, 
                             25.0, 25.0, 25.0, 28.0, 30.0, 32.0, 
                             30.0, 28.0, 24.0, 20.0, 17.5, 15.0};
*/
//pred 7.12.2014 float
const int crpRadAsAbsTemp[24] PROGMEM = {15, 15, 15, 15, 15, 15, 
                             17, 19, 22, 25, 30, 30, 
                             25, 25, 25, 28, 30, 32, 
                             30, 28, 27, 25, 20, 15};

/*
void NastaviTempCrpRad(float limitTemp[], float limitFact[], float limitAbsOnTemp[])
{
  limitTemp[0] = 30.0;
  limitTemp[1] = 30.0;
  limitTemp[2] = 30.0;
  limitTemp[3] = 30.0;
  limitTemp[4] = 30.0;
  limitTemp[5] = 30.0;
  limitTemp[6] = 28.0;
  limitTemp[7] = 15.0;
  limitTemp[8] = 12.0;
  limitTemp[9] = 13.0;
  limitTemp[10] = 15.0;
  limitTemp[11] = 17.0;
  limitTemp[12] = 19.0;
  limitTemp[13] = 21.0;
  limitTemp[14] = 20.0;
  limitTemp[15] = 18.0;
  limitTemp[16] = 15.0;
  limitTemp[17] = 12.0;
  limitTemp[18] = 14.0;
  limitTemp[19] = 16.0;
  limitTemp[20] = 18.5;
  limitTemp[21] = 22.0;
  limitTemp[22] = 30.0;
  limitTemp[23] = 30.0;
  
  limitFact[0] = 0.2;
  limitFact[1] = 0.2;
  limitFact[2] = 0.2;
  limitFact[3] = 0.2;
  limitFact[4] = 0.2;
  limitFact[5] = 0.2;
  limitFact[6] = 0.2;
  limitFact[7] = 0.2;
  limitFact[8] = 0.2;
  limitFact[9] = 0.2;
  limitFact[10] = 0.2;
  limitFact[11] = 0.2;
  limitFact[12] = 0.2;
  limitFact[13] = 0.2;
  limitFact[14] = 0.2;
  limitFact[15] = 0.2;
  limitFact[16] = 0.2;
  limitFact[17] = 0.2;
  limitFact[18] = 0.2;
  limitFact[19] = 0.2;
  limitFact[20] = 0.2;
  limitFact[21] = 0.2;
  limitFact[22] = 0.2;
  limitFact[23] = 0.2;
  
  limitAbsOnTemp[0] = 15.0;
  limitAbsOnTemp[1] = 15.0;
  limitAbsOnTemp[2] = 15.0;
  limitAbsOnTemp[3] = 15.0;
  limitAbsOnTemp[4] = 15.0;
  limitAbsOnTemp[5] = 15.0;
  limitAbsOnTemp[6] = 17.0;
  limitAbsOnTemp[7] = 19.0;
  limitAbsOnTemp[8] = 22.0;
  limitAbsOnTemp[9] = 25.0;
  limitAbsOnTemp[10] = 30.0;
  limitAbsOnTemp[11] = 30.0;
  limitAbsOnTemp[12] = 25.0;
  limitAbsOnTemp[13] = 25.0;
  limitAbsOnTemp[14] = 25.0;
  limitAbsOnTemp[15] = 28.0;
  limitAbsOnTemp[16] = 30.0;
  limitAbsOnTemp[17] = 32.0;
  limitAbsOnTemp[18] = 30.0;
  limitAbsOnTemp[19] = 28.0;
  limitAbsOnTemp[20] = 24.0;
  limitAbsOnTemp[21] = 20.0;
  limitAbsOnTemp[22] = 17.5;
  limitAbsOnTemp[23] = 15.0;
}
*/

  const char separ_str[] = ":";
  const char pika_str[] = ".";
  const char deg_str[] = "C";
  const char volt_str[] = "V";
  const char space_str[] = " ";


#endif 
