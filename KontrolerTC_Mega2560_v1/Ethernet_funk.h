#ifndef Ethernet_funk_h
#define Ethernet_funk_h


void gotPinged (byte* ptr);
void EthernetInit(boolean izpisShort);
void CheckEthernet(void);
void EthernetIzpisInfo(void);
word homePage(void);


//#define CLIENT_ADDRESS 1
//#define SERVER_ADDRESS 2
//     #define RH_RF24_MAX_MESSAGE_LEN 28
    
//RH_NRF24 driver(NRF24_CE, NRF24_CSN);
//RHReliableDatagram manager(driver, CLIENT_ADDRESS);


//------------------------------------------
static byte myip[] = {192,168,1,50}; // ethernet interface ip address
static byte gwip[] = {192,168,1,1};  // gateway ip address
static byte dnsip[] = {193,189,160,13}; // dnc ip address
static byte ipmask[] = { 255,255,252,0 }; // ip mask 
//static byte ntpip[] = {5, 9, 80, 113}; // time-a.timefreq.bldrdoc.gov
//static byte ntpip[] = {129, 6, 15, 28}; // time-a.timefreq.bldrdoc.gov

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

char website[] PROGMEM = "www.devicehub.net";

//byte Ethernet::buffer[500]; // tcp/ip send and receive buffer
byte Ethernet::buffer[700]; // tcp/ip send and receive buffer
BufferFiller bfill;

static byte ping_1[4];
static byte proc_ip[4];
static byte deviceHub_ip[4];


//-----------------------------------------------------------------
// called when a ping comes in (replies to it are automatic)
void gotPinged (byte* ptr) {
  ether.printIp(">>> ping from: ", ptr);
}


//-----------------------------------------------------------------
void EthernetInit(boolean izpisShort) {
  
  uint8_t sucess;
  
//  ether.parseIp(ping_1, (char*)"192.168.1.1");
  ether.parseIp(proc_ip, (char*)"192.168.2.3");
  
  for (int i= 0; i<5; i++) {
    
/*  
  if (izpisShort) {  
    pinMode(ETHER_CS_PIN, OUTPUT);
    digitalWrite(ETHER_CS_PIN, LOW);
    delay(250);
    digitalWrite(ETHER_CS_PIN, HIGH);
    delay(250);
  }
*/  
  if (izpisShort) {  
//    pinMode(ETHER_RESET_PIN, OUTPUT); 
    digitalWrite(ETHER_RESET_PIN, LOW);
    delay(250);
    digitalWrite(ETHER_RESET_PIN, HIGH);
//    pinMode(ETHER_RESET_PIN, INPUT); 
    delay(25);
//    digitalWrite(ETHER_RESET_PIN, HIGH);
//    delay(250);
  } 
  sucess = ether.begin(sizeof Ethernet::buffer, mymac, ETHER_CS_PIN);  
  if (sucess == 0) {
    Serial.println( "Failed to access Ethernet controller");
    continue;
  }
  else if (!izpisShort) {
    Serial.print(F("Eth. firware rev: "));
    Serial.println(sucess);
  }  
  
  if (!ether.staticSetup(myip, gwip, dnsip, ipmask)) {
    Serial.println(F("Preveri povezavo ethernet modula!"));
    continue;
  }  
  /*
  if (ether.clientWaitingGW() == false) {
    
  }  	
  */
//  sucess = ether.parseIp(ether.hisip, (char*)"192.168.1.1"); 
  sucess = ether.parseIp(ping_1, (char*)"192.168.1.1");  
  delay(5);
  if (sucess != 0) {
    Serial.print(F(" sucess: "));
    Serial.print(sucess);
//    ether.printIp(" Hisip: ", ether.hisip);
    ether.printIp(" ping_1: ", ping_1);
    
    //sucess = ether.parseIp(ether.hisip, (char*)"192.168.2.2");
    sucess = ether.parseIp(ping_1, (char*)"192.168.2.2");
    delay(5);
    if (sucess != 0) {
      Serial.print(F(" sucess: "));
      Serial.print(sucess);
//      ether.printIp(" Hisip: ", ether.hisip);
      ether.printIp(" ping_1: ", ping_1);
    
//      sucess = ether.parseIp(ether.hisip, (char*)"192.168.2.3");
      sucess = ether.parseIp(ping_1, (char*)"192.168.2.3");
      delay(5);
      if (sucess != 0) {
        Serial.print(F(" sucess: "));
        Serial.print(sucess);
//        ether.printIp(" Hisip: ", ether.hisip);
        ether.printIp(" ping_1: ", ping_1);
    
//        sucess = ether.parseIp(ether.hisip, (char*)"192.168.2.20");
        sucess = ether.parseIp(ping_1, (char*)"192.168.2.20");
        delay(5);
        if (sucess != 0) {
          Serial.print(F(" sucess: "));
          Serial.print(sucess);
//          ether.printIp(" Hisip: ", ether.hisip);
          ether.printIp(" ping_1: ", ping_1);
    
          ether.doBIST(ETHER_CS_PIN);
          delay(5);
          Serial.print(F(" sucess: "));
          Serial.print(sucess);
          continue;
        }
      }
    }    
  }
 
 if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
  else 
    ether.copyIp(deviceHub_ip, ether.hisip);
    
  ether.printIp("SRV: ", ether.hisip);
  ether.printIp("DeviceHub: ", deviceHub_ip);
    
  if (!izpisShort) {
    delay(100);
    EthernetIzpisInfo();
   
  }
  else {
    Serial.print(F(" sucess: "));
    Serial.print(sucess);
    ether.printIp("EthRein: ", ether.hisip);  
  }  
  
  
  ether.registerPingCallback(gotPinged);
  break;
  }
//  digitalWrite(ETHER_CS_PIN, LOW);
//  Serial.println(ether.packetLoopIcmpCheckReply(gwip));	
//  ether.ntpRequest(ntpip, 8888); 
//  uint32_t *t;
//  Serial.println(ether.ntpProcessAnswer(t, 0)); 		

  
  
//  ether.parseIp(addr, (char*)"192.168.2.3");

}
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
  char t7[6];
  char t8[6];
  char rh[6];
  char hum[6];
  
  dtostrf(cTemperatura[0], 2, 2, t0);
  dtostrf(cTemperatura[1], 2, 2, t1);
  dtostrf(cTemperatura[2], 2, 2, t2);
  dtostrf(cTemperatura[3], 2, 2, t3);
  dtostrf(cTemperatura[4], 2, 2, t4);
  dtostrf(cTemperatura[5], 2, 2, t5);
  dtostrf(cTemperatura[6], 2, 2, t6);
  dtostrf(cTemperatura[7], 2, 2, t7);
  dtostrf(cTemperatura[8], 2, 2, t8);
  
  dtostrf(cVlaznost[0], 2, 2, rh);
  dtostrf(cHumidex[0], 2, 2, hum);
  
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
    "<h3> T1: $SC <br> T2: $SC <br> T3: $SC <br> T4: $SC <br> T5: $SC <br> T6: $SC <br> T7: $SC <br> T8: $SC <br> T9: $SC <br>"
    "RH: $S% <br>"
    "Humidex: $SC<br>"
//    "CO: $D</h3> <br>"),
    "CO: $D<br>"
    "State: $D$D$D$D </h3> <br>"),
      itime, 
      t0, t1, t2, t3, t4, t5, t6, t7, t8,
      rh,
      hum,
      coRawValRef,
      prevCrpTCState, prevVentTCState, prevCrpRadState, stateCevStPecTC);
  return bfill.position();
}

//--------------------------------------------------------------------------------

  
 
//--------------------------------------------------------------------------------

void CheckEthernet() {
  
//  digitalWrite(ETHER_CS_PIN, HIGH);
  
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);  
//-----   
  // wait for an incoming TCP packet, but ignore its contents

/*
  if (ether.packetLoop(ether.packetReceive())) {
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }
*/  

  
  static unsigned long timerPing = millis();
  static boolean statusPing = true;
/*
  if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {
     sumPing += (millis() - timerPing); 
     numPing ++;
     statusPing = true;  
  }
  else if (millis() - timerPing > 15000 && statusPing == false) {
     EthernetInit(true);
     timerPing = millis();
     statusPing = true;
  }  
  else if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
  
  // ping a remote server once every few seconds
  
  if (millis() - timerPing >= 58750) {
   // ether.printIp("Pinging: ", ether.hisip);
    timerPing = millis();
    ether.clientIcmpRequest(ether.hisip);
    statusPing = false;
  }
  
*/  
 if (millis() - timerPing > 128750) {
   // ether.printIp("Pinging: ", ether.hisip);
//    ether.parseIp(ether.hisip, "192.168.1.1");
    timerPing = millis();
//    ether.clientIcmpRequest(ether.hisip);
    ether.clientIcmpRequest(ping_1);
    
    statusPing = false;
    do {
      len = ether.packetReceive();
      pos = ether.packetLoop(len);
//      if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip) == true) {
      if (len > 0 && ether.packetLoopIcmpCheckReply(ping_1) == true) { 
        Serial.print(F(" Ep("));
        Serial.print(len, DEC);
        Serial.print(F(";"));
        Serial.print(pos, DEC);
        Serial.print(F(") "));
//        Serial.print(F("delaycnt: "));
//        Serial.print(ether.delaycnt);
        
        if (millis() < timerPing) {
          sumPing = millis();
          numPing = 1;
        }
        else {  
          sumPing += (millis() - timerPing); 
          numPing ++;
        }
        statusPing = true;  
      } 
    }  while (statusPing == false && millis() - timerPing < 1500);
    if (statusPing == false) {
//      ether.printIp("No ping: ", ether.hisip);
      ether.printIp("No ping: ", ping_1);   
//      EthernetInit(true);
      timerPing = millis();
      statusPing = true;
      Serial.print(F("delaycnt: "));
      Serial.print((unsigned int) ether.delaycnt);
      
      if (ether.clientWaitingGw() == false) {
        Serial.print(F("Gateway IP not found "));
        ether.printIp("GW: ", ether.gwip);
//        ether.parseIp(ether.hisip, (char*)"192.168.1.1"); 
        if (!ether.staticSetup(myip, gwip, dnsip, ipmask)) {
          Serial.println(F("ReSt unsuc!"));
        } 
      }  
    }      
  }
  
  if (pos)  {  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
    Serial.print(F(" Ew("));
    Serial.print(len, DEC);
    Serial.print(F(";"));
    Serial.print(pos, DEC);
    Serial.print(F(") "));
  }
  
  
 

//  digitalWrite(ETHER_CS_PIN, LOW);
}  
  
//--------------------------------------------------------------------------------
void EthernetIzpisInfo(void) {
  
//  uint8_t sucess = 0;
  
    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip); 
    ether.printIp("DNS: ", ether.dnsip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("Broadcast: ", ether.broadcastip);
    ether.printIp("DHCP: ", ether.dhcpip);
//    Serial.print(F(" sucess: "));
//    Serial.print(sucess);
    ether.printIp("Hisip: ", ether.hisip);
    ether.printIp("ping_1 IP: ", ping_1);
    Serial.print(F("Hisport: "));
    Serial.println(ether.hisport);
    Serial.print(F("Use DHCP: "));
    if (ether.using_dhcp)
      Serial.println(F("true"));
    else
      Serial.println(F("false"));  
    Serial.print(F("persist_tcp_connection: "));
    if (ether.persist_tcp_connection)
      Serial.println(F("true"));
    else
      Serial.println(F("false"));
    Serial.print(F("delaycnt: "));
    Serial.println((unsigned int)ether.delaycnt);
    Serial.print(F(" isLinkUp:"));
    if (ether.isLinkUp() == true)
      Serial.println(F("t"));
    else
      Serial.println(F("f"));  
}

void SendToProc(void) {
  uint8_t sucess;
  
  char payload[] = "My UDP message";
  uint16_t nSourcePort = 80;
  uint16_t nDestinationPort = 5678;
  uint8_t ipDestinationAddress[4];
  sucess = ether.parseIp(ipDestinationAddress, "192.168.2.3");
  if (sucess != 0) 
      ether.printIp("Dest:", ipDestinationAddress);
  else
    ether.sendUdp(payload, sizeof(payload), nSourcePort, ipDestinationAddress, nDestinationPort);
}

boolean debugDeviceHub=false; 
// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  if (debugDeviceHub) {
    Serial.print(F(">>>"));
    Ethernet::buffer[off+300] = 0;
    Serial.print((const char*) Ethernet::buffer + off);
    Serial.print(F("..."));
//  digitalWrite(9, HIGH);
//  delay(200);
//  digitalWrite(9, LOW);
  }
}




// ime snazorja: Okolica_1 ; Id:1288 
// ime snazorja: Vlaznost_0 ; Id:1289 
void DeviceHub(void) {
  char t0[5];
  char t1[5];
  char t2[6];
  char t3[6];
  char t4[6];
  char t5[6];
  dtostrf(cTemperatura[OKOLICA_0], 2, 1, t0);
  dtostrf(cVlaznost[0], 2, 1, t1);
  dtostrf(cTemperatura[CRPALKA_0], 2, 2, t2);
  dtostrf(cTemperatura[PEC_DV], 2, 2, t3);
  dtostrf(cTemperatura[8], 2, 2, t4);
  dtostrf(cTemperatura[RAD_PV], 2, 2, t5);
//  char apiKey[] = "7b4f8725-20a1-4cdb-8937-fa3edd73cc37";
//  char apiserver[] = "www.devicehub.net";
//  ether.packetLoop(ether.packetReceive());
//  if (millis() > timer) {
 //   int chk = DHT11.read(DHT11PIN);
    char queryString[256] = {0};
//    sprintf(queryString, "?apiKey=7b4f8725-20a1-4cdb-8937-fa3edd73cc37&Okolica_1=%d&Vlaznost_0=%d", (int)cTemperatura[OKOLICA_0],(int)cVlaznost[0]);
    sprintf(queryString, "?apiKey=7b4f8725-20a1-4cdb-8937-fa3edd73cc37&Ok_0=%s&Vl_0=%s&Tc_1=%s&Pe_dv=%s&Dim_1=%s&CO_Raw=%d&Ra_pv=%s", 
                        t0, t1, t2, t3, t4, coRawValRef, t5);
//    timer = millis() + 5000;
//    Serial.println();
    Serial.print(F("<<< REQ"));

    ether.browseUrl(PSTR("/io/775/"), queryString, website, my_callback);

//  }
}  

/*
void ProcessingHub(void) {
  char t0[5];
  char t1[5];
  char t2[6];
  char t3[6];
  char t4[6];
  char t5[6];
  dtostrf(cTemperatura[OKOLICA_0], 2, 1, t0);
  dtostrf(cVlaznost[0], 2, 1, t1);
  dtostrf(cTemperatura[CRPALKA_0], 2, 2, t2);
  dtostrf(cTemperatura[PEC_DV], 2, 2, t3);
  dtostrf(cTemperatura[8], 2, 2, t4);
  dtostrf(cTemperatura[RAD_PV], 2, 2, t5);
//  char apiKey[] = "7b4f8725-20a1-4cdb-8937-fa3edd73cc37";
//  char apiserver[] = "www.devicehub.net";
//  ether.packetLoop(ether.packetReceive());
//  if (millis() > timer) {
 //   int chk = DHT11.read(DHT11PIN);
    char queryString[256] = {0};
//    sprintf(queryString, "?apiKey=7b4f8725-20a1-4cdb-8937-fa3edd73cc37&Okolica_1=%d&Vlaznost_0=%d", (int)cTemperatura[OKOLICA_0],(int)cVlaznost[0]);
    sprintf(queryString, "?apiKey=7b4f8725-20a1-4cdb-8937-fa3edd73cc37&Ok_0=%s&Vl_0=%s&Tc_1=%s&Pe_dv=%s&Dim_1=%s&CO_Raw=%d&Ra_pv=%s", 
          t0, t1, t2, t3, t4, coRawValRef, t5);
//    timer = millis() + 5000;
//    Serial.println();
    Serial.print(F("<<< REQ"));
    
    ether.browseUrl(PSTR("/io/775/"), queryString, website, my_callback);

//  }
}  
*/

#endif
