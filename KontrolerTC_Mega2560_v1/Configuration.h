#ifndef Configuration_h
#define Configuration_h

//#define INTFACT
#define VERSION "v1.004"
#define MAXSENSORS 7     //maksimalno stevilo temp sensorjev DS18x20

//----------
#define NED  1    //nedelja, kot rezltat weekday()...
#define SOB  7    //sobota, .....

//----------
//in/out state definition
#define R_TC_ON   HIGH      // z RELE_TC
#define R_TC_OFF  LOW

#define R_CRAD_ON   LOW      // z RELE_CRAD
#define R_CRAD_OFF  HIGH

#define R_CTC_ON   LOW      //z RELE_CTC 
#define R_CTC_OFF  HIGH

#define CEV_TERM_ON   LOW
#define CEV_TERM_OFF  HIGH

//----------
// pin and address definition


#define SENS_TOK     A0   //senzor toka na A0
#define SENS_V12     A8
#define SENS_V5_2    A9
#define SENS_RTC_BATT A10

#define ENC_CLK_PIN   2     // Used for generating interrupts using CLK signal - interuppt 0
#define ENC_DT_PIN    4     // Used for reading DT signal
#define ENC_SW_PIN    5           // Used for the push button switch

#define ETHER_CS_PIN  10

#define LCD_OSW_SW    7
#define LCD_RS    22
#define LCD_RW    23
#define LCD_EN    24
#define LCD_D4    25
#define LCD_D5    26
#define LCD_D6    27
#define LCD_D7    28

#define BEEP_PIN      29
#define ONE_WIRE_BUS 30    //1-wire na pin D4 - D30

#define CEVTERM_PEC_TC  32  // cevni termostat na peci proti TC
//#define stikalo TC_ON 33
//#define stikalo TC_OFF 34
//#define stikalo_Crp_Rad_On 35
//#define stikalo Crp_Rad_Off 36
//#define stikalo Crp_TC_On 37
//#define stikalo Crp_TC_On 38

//#define Rele 4/4  39
//#define Rele 3/4  40
#define RELE_CTC    41    // rele 2/4 za crpalko na TC
#define RELE_CRAD   42    // rele 1/4 crpalke radijatorjev
#define RELE_TC     43    //rele 1 proklopljen na pin D7 -> D40 - vklop  TC

//#define L298 En2  44    //ENB
//#define L298 1/2  45    //l298 4/4 
//#define L298 2/2  46    //l298 3/4
#define VENTTC_1    47    // na L298  - input 1
#define VENTTC_2    48    // na L293D - input 2
#define VENTTC_EN   49    // ENA na L293D - enable input 1,2  

#define SD_CS_PIN  53





//----------
#define DS1307_CTRL_ID 0x68 
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message


//----------
#define AT24C32_I2C_ADDR 0x50

#define AT24C32_MAX_ADDR 4096
#define AT24C32_ADDR_LENGH 2

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
#define  OKOLICA_0  0         // index enzorja okolice
#define  CRPALKA_0  1         // index sensorja na crpalki
#define  PEC_PV     2   // pec, povratni vod
#define  PEC_DV     3   // pecc, dvizni vod
#define  RAD_PV     4   // radijatorji, povratni vod
#define  RAD_DV     5   // radijatorji, dvizni vod
#define  PEC_TC_DV  6 // dvizni vod proti TC

//------------------------------------------
static byte myip[] = {192,168,1,50}; // ethernet interface ip address
static byte gwip[] = {192,168,1,1};  // gateway ip address
static byte dnsip[] = {193,189,160,13}; // dnc ip address
//static byte ipmask[] = { 255,255,252,0 }; // ip mask 
//static byte ntpip[] = {5, 9, 80, 113}; // time-a.timefreq.bldrdoc.gov
//static byte ntpip[] = {129, 6, 15, 28}; // time-a.timefreq.bldrdoc.gov

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
BufferFiller bfill;



//------------------------------------------
boolean measureOnly = false;
boolean showCRC = true;  // izpis kontrolnih parametrov na serial

//------------------------------------------




//------------------------------------------

unsigned int   merXMin = 1;          //temperaaturo merimo vsakih merXMin minut
float minTempVTOn = 37.5;    //min temp- vode za vklop pri visoki tarifi t < 
int   minRunTimeMin = 10;    // minimalni cas (delovanja)  ali ne-delovanja TC / (odvisno od UpostevajElTarife(void)) - ne vec


float dTemp = 5.0;  // default 5.0
float sensDiff = 2.5;

//------------------------------------------
float minTempNightOn = 32.5;

//------------------------------------------
float ciljnaTemp = 50;
//float deltaTh = 5.55;   // do 16.Oct.2012
//float deltaTh = 4.35;   //
float deltaTh;
float deltaThOk;
float deltaThSt;
const float tKompOK = 20.0;
//const float tKompSt = 20.0;
const float tKompSt = 50.0;

//------------------------------------------
int uraVTemp[] = {6, 21};  //obdobje vklopa pri temp minTempVTOn - 0-start, 1-end
//int vStartUraOff = 20;     //ura zacetka znizevanja temerature za izklop
int dolPrehObd = 121;     //dolzina prehodnega obdobja postopnega znizevanja temp. za izklop (v minutah)
int startUraVT = 6;  // zacetek visoke tarife
int startUraNT = 22; // zacetek nizke tarife :)
float mejaToka = 1.0;

//------------------------------------------
// boolean upostevajElTarife = false; // glej opombo pod verzijo v0.45 && v0.54
//povezava crpalka - pec 
float tempVklopaCrpTC = 82.5 ; //best = 75.0;       // temp. vklopa crpalke TC
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
unsigned long lastCrpRadStateChg;

//------------------------------------------
//float limTempCrpRad[]
//float limTempFactCrpRad[24];
float minMejnaTempRel  = 0.399;

float maxTempPVRad = 50.0; // max temperatura povratnega voda
float maxTempDVPec = 90.01; // max. temperatura dviznega voda peci
float maxDeltaDev = 5.0;
//------------------------------------------
num2byte4b u4;
//------------------------------------------
unsigned int addrLastChg = 0;
unsigned int addrOnTime = 5;
unsigned int addrDeltaTh = 9;
unsigned int addrDeltaThOk = 13;
unsigned int addrDeltaThSt = 17;

unsigned int addrLastHourTemp = 128; // 500
unsigned int addrTempBack = 968;     //1024 
//-----------------------------------------
unsigned int histLen = 168;
int zapisXMin = 60;
num2byte4f uf;
//--------------------------------------





typedef uint8_t DeviceAddress[8];
//definicija naslovov senzorjev
int FiksAdrrSens(DeviceAddress devAddress[], byte *type_s)
{
/*
//  normal
  // okolica
  devAddress[0][0] = 0x28;
  devAddress[0][1] = 0xC3;
  devAddress[0][2] = 0x68;
  devAddress[0][3] = 0x97;
  devAddress[0][4] = 0x03;
  devAddress[0][5] = 0x00;
  devAddress[0][6] = 0x00;
  devAddress[0][7] = 0x74;
  type_s[0] = 0;

// okolica Na DS1307
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0xC2;
  devAddress[1][2] = 0x3A;
  devAddress[1][3] = 0x97;
  devAddress[1][4] = 0x03;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0x55;
  type_s[0] = 0;
  
  // crpalka
//  devAddress[1][0] = 0x28;
//  devAddress[1][1] = 0x32;
//  devAddress[1][2] = 0x5B;
//  devAddress[1][3] = 0x97;
//  devAddress[1][4] = 0x03;
//  devAddress[1][5] = 0x00;
//  devAddress[1][6] = 0x00;
//  devAddress[1][7] = 0xFB;
//  type_s[1] = 0;
  
  
  //pec - povratni - prej na cevi TC
  devAddress[2][0] = 0x28;
  devAddress[2][1] = 0x99;
  devAddress[2][2] = 0x73;
  devAddress[2][3] = 0x97;
  devAddress[2][4] = 0x03;
  devAddress[2][5] = 0x00;
  devAddress[2][6] = 0x00;
  devAddress[2][7] = 0x99;
  type_s[2] = 0;
  
  
 
  
  
   // pec dvizni vod
  devAddress[3][0] = 0x28;
  devAddress[3][1] = 0x32;
  devAddress[3][2] = 0x7C;
  devAddress[3][3] = 0x97;
  devAddress[3][4] = 0x03;
  devAddress[3][5] = 0x00;
  devAddress[3][6] = 0x00;
  devAddress[3][7] = 0x52;
  type_s[3] = 0;
  
   // povratni vod radiatorji
  devAddress[4][0] = 0x28;
  devAddress[4][1] = 0xEA;
  devAddress[4][2] = 0x3A;
  devAddress[4][3] = 0x97;
  devAddress[4][4] = 0x03;
  devAddress[4][5] = 0x00;
  devAddress[4][6] = 0x00;
  devAddress[4][7] = 0x42;
  type_s[4] = 0;
  
   // dvizni vod radiatorji
  devAddress[5][0] = 0x28;
  devAddress[5][1] = 0x02;
  devAddress[5][2] = 0x8B;
  devAddress[5][3] = 0x97;
  devAddress[5][4] = 0x03;
  devAddress[5][5] = 0x00;
  devAddress[5][6] = 0x00;
  devAddress[5][7] = 0x48;
  type_s[5] = 0;
  
  
  devAddress[6][0] = 0x28;
  devAddress[6][1] = 0xD7;
  devAddress[6][2] = 0x85;
  devAddress[6][3] = 0x97;
  devAddress[6][4] = 0x03;
  devAddress[6][5] = 0x00;
  devAddress[6][6] = 0x00;
  devAddress[6][7] = 0xC5;
  type_s[6] = 0;
*/  
  
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
  type_s[0] = 0;

// okolica Na DS1307
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0xC2;
  devAddress[1][2] = 0x3A;
  devAddress[1][3] = 0x97;
  devAddress[1][4] = 0x03;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0x55;
  type_s[0] = 0;
 /* 
  // crpalka
  devAddress[1][0] = 0x28;
  devAddress[1][1] = 0x32;
  devAddress[1][2] = 0x5B;
  devAddress[1][3] = 0x97;
  devAddress[1][4] = 0x03;
  devAddress[1][5] = 0x00;
  devAddress[1][6] = 0x00;
  devAddress[1][7] = 0xFB;
  type_s[1] = 0;
*/  
  
  //pec - povratni - prej na cevi TC
  devAddress[2][0] = 0x28;
  devAddress[2][1] = 0xA4;
  devAddress[2][2] = 0x44;
  devAddress[2][3] = 0x9F;
  devAddress[2][4] = 0x04;
  devAddress[2][5] = 0x00;
  devAddress[2][6] = 0x00;
  devAddress[2][7] = 0x8D;
  type_s[2] = 0;
  
  
 
  
  
   // pec dvizni vod
  devAddress[3][0] = 0x28;
  devAddress[3][1] = 0xAF;
  devAddress[3][2] = 0xD9;
  devAddress[3][3] = 0x9E;
  devAddress[3][4] = 0x04;
  devAddress[3][5] = 0x00;
  devAddress[3][6] = 0x00;
  devAddress[3][7] = 0xA1;
  type_s[3] = 0;
  
   // povratni vod radiatorji
  devAddress[4][0] = 0x28;
  devAddress[4][1] = 0x31;
  devAddress[4][2] = 0x38;
  devAddress[4][3] = 0x9F;
  devAddress[4][4] = 0x04;
  devAddress[4][5] = 0x00;
  devAddress[4][6] = 0x00;
  devAddress[4][7] = 0x9B;
  type_s[4] = 0;
  
   // dvizni vod radiatorji
  devAddress[5][0] = 0x28;
  devAddress[5][1] = 0xB3;
  devAddress[5][2] = 0xD7;
  devAddress[5][3] = 0x9E;
  devAddress[5][4] = 0x04;
  devAddress[5][5] = 0x00;
  devAddress[5][6] = 0x00;
  devAddress[5][7] = 0x25;
  type_s[5] = 0;
  
  
  devAddress[6][0] = 0x28;
  devAddress[6][1] = 0x67;
  devAddress[6][2] = 0xA1;
  devAddress[6][3] = 0x9E;
  devAddress[6][4] = 0x04;
  devAddress[6][5] = 0x00;
  devAddress[6][6] = 0x00;
  devAddress[6][7] = 0xCC;
  type_s[6] = 0;
  
  /*
  switch (addr[0]) {
      case 0x10:
        Serial.println(" Chip=DS18S20");  // or old DS1820
        type_s[numSens-1] = 1;
        break;
      case 0x28:
        Serial.println(" Chip=DS18B20");
        type_s[numSens-1] = 0;
        break;
      case 0x22:
        Serial.println(" Chip=DS1822");
        type_s[numSens-1] = 0;
        break;
      default:
        Serial.println("Device is not a DS18x20 family device.");
        return;
    }
  */  
  
  return (7);
}  
int limTempCrpRad[24] = {30, 30, 30, 30, 30, 30, 
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
float limTempFactCrpRad[24] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 
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
int crpRadAsAbsTemp[24] = {15, 15, 15, 15, 15, 15, 
                             17, 19, 22, 25, 30, 30, 
                             25, 25, 25, 28, 30, 32, 
                             30, 28, 24, 20, 17, 15};

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

#endif 
