#ifndef Ethernet_funk_h
#define Ethernet_funk_h

#define DEBUG_ETHCRD
//#define DEBUG_ETHCRD_IP
//#define DEBUG_CALLBACK

void gotPinged (byte* ptr);
void EthernetInit(boolean izpisShort);
void CheckEthernet(void);
void EthernetIzpisInfo(void);
word homePage(void);
unsigned long getNtpTime(void);

void printDomo_EthCard(boolean newCycle_Domo);



//------------------------------------------
static byte myip[] = {192, 168, 178, 50}; // ethernet interface ip address
static byte gwip[] = {192, 168, 178, 1}; // gateway ip address
static byte dnsip[] = {193, 189, 160, 13}; // dnc ip address
static byte ipmask[] = { 255, 255, 255, 0 }; // ip mask


// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };



const char website_Things[] PROGMEM = "api.thingspeak.com";
#define APIKEY  "CB9RI4WF29AG0QF1"
Stash stash;

//const char website_Domoticz[] PROGMEM = "192.168.178.20";
//int port_Domoticz = 8080; //Domoticz port
static byte ip_Domot[] = {192, 168, 178, 20}; // ip


byte Ethernet::buffer[700]; // tcp/ip send and receive buffer
BufferFiller bfill;

//static byte ping_1[4];
static byte proc_ip[4];
static byte deviceHub_ip[4];
static byte deviceHubMQTT_ip[4];
static byte thingSpeak_ip[4];

const char ntpServer[] PROGMEM = "ntp.siol.net";
static byte ntp_ip[4];


//-----------------------------------------------------------------
// called when a ping comes in (replies to it are automatic)
void gotPinged (byte* ptr) {
  ether.printIp(">>> ping from: ", ptr);
}


//-----------------------------------------------------------------
void EthernetInit(boolean izpisShort) {

  uint8_t sucess;

  ether.parseIp(proc_ip, (char*)"192.168.178.1");

  for (int i = 0; i < 5; i++) {
    if (izpisShort) {
     /*
      digitalWrite(ETHER_RESET_PIN, LOW);
      delay(250);
      digitalWrite(ETHER_RESET_PIN, HIGH);
      delay(25);
    */
    }
    
    sucess = ether.begin(sizeof Ethernet::buffer, mymac, ETHER_CS_PIN);
    if (sucess == 0) {
      Serial.println(F("Failed to access Ethernet controller"));
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

 
  if (!ether.dnsLookup(ntpServer)) {
    Serial.println( F("DNS failed" ));
  } else {
    ether.copyIp(ntp_ip, ether.hisip);
    ether.printIp("NTP: ", ether.hisip);
    ether.printIp("NTP: ", ntp_ip);
  }



#ifdef USEDEVICEHUB
      if (!ether.dnsLookup(website_DeviceHub))
        Serial.println(F("DNS failed"));
      else {
        ether.copyIp(deviceHub_ip, ether.hisip);
        ether.printIp("SRV: ", ether.hisip);
        ether.printIp("DeviceHub: ", deviceHub_ip);
      }
 #endif
 #ifdef USEDEVICEHUB_MQTT   
      if (!ether.dnsLookup(website_DeviceHubMQTT))
        Serial.println(F("DNS failed"));
      else {
        ether.copyIp(deviceHubMQTT_ip, ether.hisip);
        ether.printIp("SRV: ", ether.hisip);
        ether.printIp("DeviceHubMQTT: ", deviceHubMQTT_ip);
      }
#endif
#ifdef USETHINGSPEAK
      if (!ether.dnsLookup(website_Things)) {
        Serial.println(F("DNS failed"));
      }
      else {
        ether.copyIp(thingSpeak_ip, ether.hisip);
        ether.printIp("SRV: ", ether.hisip);
        ether.printIp("Things: ", thingSpeak_ip);
      }
#endif

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

}


static word homePage() {

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
                 "State: $D$D$D$D$D$D$D   </h3> <br>"),
               itime,
               t0, t1, t2, t3, t4, t5, t6, t7, t8,
               rh,
               hum,
               coRawValRef,
               prevTCState, !releState_ventKompTC, !releState_kompTC, prevCrpTCState, prevVentTCState, prevCrpRadState, !stateCevStPecTC);
  return bfill.position();
}



//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------


void CheckEthernet_originalWebpage() {

  //  digitalWrite(ETHER_CS_PIN, HIGH);
  //ether.packetLoop(ether.packetReceive());
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  //-----
  // wait for an incoming TCP packet, but ignore its contents


  static unsigned long timerPing = millis();
  static boolean statusPing = true;
  
  if (pos)  {  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
    Serial.print(F(" Ew("));
    Serial.print(len, DEC);
    Serial.print(F(";"));
    Serial.print(pos, DEC);
    Serial.print(F(") "));
  }
}

void CheckEthernet() {

//  ether.packetLoop(ether.packetReceive());
//  printDomo_EthCard(false);
  //  digitalWrite(ETHER_CS_PIN, HIGH);
//  printDomo_EthCard(false);
  ether.packetLoop(ether.packetReceive());

}

boolean initDomotEth = false;

CheckEthernet_II()
{
  
  if (initDomotEth) {
//    printDomo_EthCard(false);
  }
  ether.packetLoop(ether.packetReceive());
}
//--------------------------------------------------------------------------------
void EthernetIzpisInfo(void) {

  //  uint8_t sucess = 0;
  Serial.println("Ethercard ");
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("Broadcast: ", ether.broadcastip);
  ether.printIp("DHCP: ", ether.dhcpip);
  //    Serial.print(F(" sucess: "));
  //    Serial.print(sucess);
  ether.printIp("Hisip: ", ether.hisip);
//  ether.printIp("ping_1 IP: ", ping_1);
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
  static char proc_ip[] = {192, 168, 178, 25};
  sucess = ether.parseIp(ipDestinationAddress, proc_ip);
  if (sucess != 0)
    ether.printIp("Dest:", ipDestinationAddress);
  else
    ether.sendUdp(payload, sizeof(payload), nSourcePort, ipDestinationAddress, nDestinationPort);
}


// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  if (debugDeviceHub) {
    Serial.print(F(">>>"));
    Ethernet::buffer[off + 300] = 0;
    Serial.print((const char*) Ethernet::buffer + off);
    Serial.print(F("..."));
    //  digitalWrite(9, HIGH);
    //  delay(200);
    //  digitalWrite(9, LOW);
  }
}









byte session;

void ThingSpeakUpdate(void) {
  
 #ifdef DEBUG_ETHCRD_IP
    Serial.println();
    ether.printIp("Hisip Tb: ", ether.hisip);
    Serial.print("hisport: ");
    Serial.print(ether.hisport);
#endif    
    ether.copyIp(ether.hisip,thingSpeak_ip);
//    ether.hisport = 80;
#ifdef DEBUG_ETHCRD_IP
    ether.printIp("Hisip Ta: ", ether.hisip);
    Serial.print("hisport: ");
    Serial.print(ether.hisport);
#endif    

  ether.packetLoop(ether.packetReceive());
  
  char t0[6];
  char t1[6];
  char t2[6];
  char t3[6];
  char t4[6];
  char t5[6];
  char t6[6];
  char t7[6];
  dtostrf(cTemperatura[0], 2, 2, t0);
  dtostrf(cTemperatura[CRPALKA_0], 2, 2, t1);
  dtostrf(cTemperatura[PEC_PV], 2, 2, t2);
  dtostrf(cTemperatura[PEC_DV], 2, 2, t3);
  dtostrf(cTemperatura[RAD_PV], 2, 2, t4);
  dtostrf(cTemperatura[RAD_DV], 2, 2, t5);
  dtostrf(cTemperatura[PEC_TC_DV], 2, 2, t6);
  dtostrf(cTemperatura[OKOLICA_0], 2, 2, t7);

  byte sd = stash.create();
  stash.print("field1=");
  stash.println(t0);
  stash.print("&field2=");
  stash.println(t1);
  stash.print("&field3=");
  stash.println(t2);
  stash.print("&field4=");
  stash.println(t3);
  stash.print("&field5=");
  stash.println(t4);
  stash.print("&field6=");
  stash.println(t5);
  stash.print("&field7=");
  stash.println(t6);
  stash.print("&field8=");
  stash.println(t7);
  stash.save();



  // generate the header with payload - note that the stash size is used,
  // and that a "stash descriptor" is passed in as argument using "$H"

  Stash::prepare(PSTR("POST /update HTTP/1.1" "\r\n"
                      "Host: $F" "\r\n"
                      "Connection: close" "\r\n"
                      "X-THINGSPEAKAPIKEY: $F" "\r\n"
                      "Content-Type: application/x-www-form-urlencoded" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "\r\n"
                      "$H"),
                 website_Things, PSTR(APIKEY), stash.size(), sd);

  // send the packet - this also releases all stash buffers once done

  session = ether.tcpSend();
//delay(25);
#ifdef DEBUG_ETHCRD
  Serial.print(session);
  Serial.print(" ");
  Serial.print(sd);
  Serial.print(" ");
  Serial.print(stash.size());
#endif

  
  int freeCount = stash.freeCount();
  if (freeCount <= 3) {   
    Stash::initMap(56); 
  } 


  const char* reply = ether.tcpReply(session); 
  
#ifdef DEBUG_ETHCRD
  if (reply != 0) {
     //res = 0;    //glej: https://github.com/jcw/ethercard/blob/master/examples/thingspeak/thingspeak.ino
     Serial.print(F(" >>>REPLY ThS.: "));
     Serial.print(reply);  
  }
#endif  
  Serial.print(F("<< ThS"));
}


//
/////////////////////////////////////////////////////////////
//
//_tcpSend
void printDomo_EthCard_tcpSend() {
  const char *domoticz_server = "192.168.178.20";
  const int dstPort PROGMEM = 8080;
 // const int srcPort PROGMEM = 124;

#ifdef DEBUG_ETHCRD_IP
    Serial.println();
    ether.printIp("Hisip Db: ", ether.hisip);
    Serial.print("hisport: ");
    Serial.print(ether.hisport);
#endif
    ether.copyIp(ether.hisip, ip_Domot);
    ether.hisport = 8080;
#ifdef DEBUG_ETHCRD_IP    
    ether.printIp("Hisip Da: ", ether.hisip);
    Serial.print("hisport: ");
    Serial.print(ether.hisport);
#endif

    byte sd = stash.create();
    stash.print("http://192.168.178.20:8080");
    stash.print("/json.htm");
    stash.print("?type=command&param=udevice&idx=");
    stash.print("3");
    stash.print("&nvalue=0&svalue=");
    stash.println("40.01");
    stash.println();
    stash.save();
    
    
     Stash::prepare(PSTR("HTTP/1.0" "\r\n"
                        "Host: $F:$D" "\r\n"                     
                        "User-Agent: Arduino-ethernet" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "Connection: close" "\r\n"
                        "\r\n"
                        "$H"),
             domoticz_server, dstPort, stash.size(), sd);
   
    for (int i; i<stash.size(); i++)
      Serial.print(stash.get());
   
   byte session = ether.tcpSend();

  delay(25);
#ifdef DEBUG_ETHCRD
  Serial.print(session);
  Serial.print(" ");
  Serial.print(sd);
  Serial.print(" ");
  Serial.print(stash.size());
#endif

  
  int freeCount = stash.freeCount();
  if (freeCount <= 3) {   
    Stash::initMap(56); 
  } 


  const char* reply = ether.tcpReply(session);
  
#ifdef DEBUG_ETHCRD
   Serial.print("/");
   Serial.print(reply);
   Serial.print("/"); 
  if (reply != 0) {
     //res = 0;    //glej: https://github.com/jcw/ethercard/blob/master/examples/thingspeak/thingspeak.ino
     Serial.print(F(" >>>REPLY Dom.: "));
     Serial.print(reply);  
  }
#endif  
ether.hisport = 80;
}



//unsigned long dom_callback_dur;
unsigned long lastDomo_sendTime = millis();
boolean lastDomo_cb_received = true;

static void dom_callback(byte status, word off, word len) {

  Ethernet::buffer[off + len] = 0;
  if (status) {
    Serial.print(F(" cb.Bad "));
  }
#ifdef DEBUG_CALLBACK
  Serial.print(F("cb:"));
  Serial.print(status);

  Serial.print(space_str);
  Serial.print(millis() - lastDomo_sendTime);
  Serial.println(F("ms"));
  Serial.println((char *)(Ethernet::buffer+off));
#endif
  char * pch;
  pch = strstr ((char *)(Ethernet::buffer+off),"ERR");
  if (pch != NULL) {
    Serial.print(F(" cb.ERR"));
    return;
  }  
  lastDomo_cb_received = true;
  lastDomo_sendTime = millis();
/*
    Serial.print(F("cb3:"));
    Serial.print(status);
    Serial.print(" ");
    Serial.print(millis() - lastDomo_sendTime);
    Serial.print(F("ms "));
    Ethernet::buffer[off + len] = 0;
    lastDomo_cb_received = true;
#ifdef DEBUG_CALLBACK
    /// ??
    Serial.print(F("off:"));
    Serial.print(off);
    Serial.print(F(" len:"));
    Serial.print(len);
    Serial.print(F(" Buff + off:"));
    Serial.println((const char*) Ethernet::buffer + off);
//    Serial.print(" Buff:");
//    for (int i=0; i<off;i++) {
//      Serial.write((const char*) Ethernet::buffer + i);
//    }
    
//    Serial.println((const char*) Ethernet::buffer);
#endif
 */ 
}

static void dom_callback_4(byte status, word off, word len) {
 Ethernet::buffer[off + len] = 0;
  Serial.print(F("cb4:"));
  Serial.print(status);
#ifdef DEBUG_CALLBACK
  Serial.print(space_str);
  Serial.print(millis() - lastDomo_sendTime);
  Serial.println(F("ms"));
  Serial.println((char *)(Ethernet::buffer+off));
#endif
  char * pch;
  pch = strstr ((char *)(Ethernet::buffer+off),"ERR");
  if (pch != NULL)
    return;
  lastDomo_cb_received = true;
  lastDomo_sendTime = millis();
}



void printDomo_T_EthCard_browseUrl(int index, float temper) {
//  const char * domoticz_server = "192.168.178.20";

    char temp[6];
    dtostrf(temper, 2, 2, temp);

//     ether.enc_freemem();
    char queryString[60];
     sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();     
/*
    if (index == 4) {
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, " ", dom_callback);
      lastDomo_sendTime = millis();     
    }
    else if (index == 5) {
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, " ", dom_callback_4); 
      lastDomo_sendTime = millis();    
    }
*/
#ifdef DEBUG_ETHCRD    
    Serial.print(F("Dom"));
    Serial.print(index);
    Serial.print(F(": "));
#ifdef DEBUG_CALLBACK    
    Serial.print(queryString);
    Serial.print(space_str);
#endif    
    Serial.print(sizeof(queryString));
#endif 

/*    unsigned long timeout = millis() + 45000UL;
    while(!dom_rep && millis() < timeout) {
      delay(10);
      ether.browseUrl("", "", " ", dom_callback); 
    }  
*/
  
}


unsigned long cb_timeout = 30000;
uint8_t dataSendSelector = 0;

 char temp[6];
 char hum[6];
 char queryString[70];
uint8_t index;
uint8_t maxData = 9; 


void printDomo_EthCard(boolean newCycle_Domo) {

  boolean resendLastData = false;
  
   if (ether.isLinkUp() == false) {
    delay(cb_timeout);
    return;
  }     

//  if (now() - dom_lastOK > 180UL) {
//    EthernetInit(true);
//}   
  
#ifdef DEBUG_ETHCRD_IP
    Serial.println();
    ether.printIp("Hisip Db: ", ether.hisip);
    Serial.print(F("hisport: "));
    Serial.print(ether.hisport);
#endif
    ether.copyIp(ether.hisip, ip_Domot);
    ether.hisport = 8080;
#ifdef DEBUG_ETHCRD_IP    
    ether.printIp("Hisip Da: ", ether.hisip);
    Serial.print(F("hisport: "));
    Serial.print(ether.hisport);
#endif

  ether.packetLoop(ether.packetReceive());  

  if (newCycle_Domo && lastDomo_cb_received) {
    dataSendSelector ++;
    if (dataSendSelector >= maxData) {
      dataSendSelector = 0;     
    }    
  }
  if (lastDomo_cb_received && !newCycle_Domo) {
    if (dataSendSelector < 255) {
      dataSendSelector ++;
    }      
  }
  if (((millis() - lastDomo_sendTime) > cb_timeout) || (lastDomo_sendTime > millis())) {
    resendLastData = true;
  }
  
  
  if (lastDomo_cb_received || resendLastData) {
    if (dataSendSelector == 0) {
      lastDomo_cb_received = false;
      index = 4;
      dtostrf(cTemperatura[CRPALKA_0], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();
#ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
#endif 
      Serial.print(F(" freeMemory:"));
      Serial.print(freeMemory());            
    }
    else if (dataSendSelector == 1){
      lastDomo_cb_received = false;
//      printDomo_T_EthCard_browseUrl(5, cTemperatura[0]);   
      
      index = 5;
      dtostrf(cTemperatura[0], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
    else if (dataSendSelector == 2){
      lastDomo_cb_received = false;
      index = 6;
      dtostrf(cTemperatura[PEC_PV], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
     else if (dataSendSelector == 3){
      lastDomo_cb_received = false;
      index = 7;
      dtostrf(cTemperatura[PEC_DV], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
     else if (dataSendSelector == 4){
      lastDomo_cb_received = false;
      index = 8;
      dtostrf(cTemperatura[RAD_PV], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
    else if (dataSendSelector == 5){
      lastDomo_cb_received = false;
      index = 9;
      dtostrf(cTemperatura[RAD_DV], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
     else if (dataSendSelector == 6){
      lastDomo_cb_received = false;
      index = 10;
      dtostrf(cTemperatura[PEC_TC_DV], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
    else if (dataSendSelector == 7){
      lastDomo_cb_received = false;
      index = 11;
      dtostrf(cTemperatura[8], 2, 2, temp);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s",index, temp);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
    else if (dataSendSelector == 8){
      lastDomo_cb_received = false;
      index = 12;
      dtostrf(cTemperatura[OKOLICA_0], 2, 2, temp);
      dtostrf(cVlaznost[0], 2, 2, hum);
      sprintf(queryString, "?type=command&param=udevice&idx=%d&nvalue=0&svalue=%s;%s;0",index, temp, hum);
      ether.browseUrl(PSTR("/json.htm"), queryString, "", dom_callback);
      lastDomo_sendTime = millis();   
  #ifdef DEBUG_ETHCRD      
      Serial.print(F(" D:"));
      Serial.print(index);  
  #endif   
    }
    else {
 //     ether.packetLoop(ether.packetReceive());
    }
  }
//  ether.packetLoop(ether.packetReceive());
// ether.hisport = 80;
//    Serial.print("sel:");
//    Serial.print(dataSendSelector);
//    Serial.print(" ");  
//  Serial.print(F("Free RAM: "));
//  Serial.println(FreeRam());

}



//=========================== NTP ===========================
uint8_t ntpMyPort = 123;


//unsigned long timeZoneOffset = 3600L;
unsigned long timeZoneOffset = SECS_PER_HOUR * TIME_ZONE;



//----------------------------------------------------------
unsigned long getNtpTime(void) {
  unsigned long timeFromNTP;
  const unsigned long seventy_years = 2208988800UL;
  unsigned long timeOut = 60000;
  
  if (ether.isLinkUp() == false) {
    delay(timeOut);
    return 0;
  }

  if (lastDomo_cb_received == false) {
    return 0;
  }
  
  ether.ntpRequest(ntp_ip, ntpMyPort);
  Serial.println("NTP request sent"); 
  unsigned long timeEnd = millis() +  timeOut;
  while(millis() <  timeEnd) {
      word length = ether.packetReceive();
      ether.packetLoop(length);
      if(length > 0) {
        if (ether.ntpProcessAnswer(&timeFromNTP,ntpMyPort)) {
          Serial.println("NTP reply received");
          return (timeFromNTP - seventy_years + timeZoneOffset + dayLightSaving * SECS_PER_HOUR);      
        }
      }
  }
  
  return 0;
}






#endif


/*

// Demo of multiple browseUrl requests
// By Roger Clark. 
// www.rogerclark.net
// 2012/11/10
// http://opensource.org/licenses/mit-license.php
//
// This demo retrieves two different pages on the same website


#include "EtherCard.h"


// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
char website[] PROGMEM = "www.rogerclark.net";
byte Ethernet::buffer[700];
static uint32_t timer;
int state;

#define TIMEOUT_MS 5000

// Called for each packet of returned data from the call to browseUrl (as persistent mode is set just before the call to browseUrl)
static void browseUrlCallback1 (byte status, word off, word len) 
{
   Ethernet::buffer[off+len] = 0;// set the byte after the end of the buffer to zero to act as an end marker (also handy for displaying the buffer as a string)
   
   Serial.println("Callback 1");
   Serial.println((char *)(Ethernet::buffer+off));
   state=3;// Move to state that causes next browseUrl
 }
 
 static void browseUrlCallback2 (byte status, word off, word len) 
{
   Ethernet::buffer[off+len] = 0;// set the byte after the end of the buffer to zero to act as an end marker (also handy for displaying the buffer as a string)

   Serial.println("Callback 2");
   Serial.println((char *)(Ethernet::buffer+off));

   state=5;// go back to waiting state
 }

void setup () 
{
  Serial.begin(115200);
  Serial.println("\n[Multiple browseUrl request example");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
  {
    Serial.println( "Error:Ethercard.begin");
    while(true);
  }

  if (!ether.dhcpSetup())
  {
    Serial.println("DHCP failed");
    while(true);
  }
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip); 

#if 0
  // Wait for link to become up - this speeds up the dnsLoopup in the current version of the Ethercard library
  while (!ether.isLinkUp())
  {
      ether.packetLoop(ether.packetReceive());
  }
#endif
long t=millis();
  if (!ether.dnsLookup(website,false))
  {
    Serial.println("DNS failed. Unable to continue.");
    while (true);
  }
  Serial.println(millis()-t);
  ether.printIp("SRV: ", ether.hisip);
  
  state=0;
}

void loop () 
{
  byte len;
  //ether.packetLoop(ether.packetReceive());
  switch (state)
  {
    case 0:
    if (millis() > timer) 
    {
    timer = millis() + 15000;// every 30 secs
    state=1;
    }

    break;
     case 1:
      while(ether.packetLoop(ether.packetReceive()));

      Serial.println("\nSending request for page /webservices/dateUTC.php");

      ether.browseUrl(PSTR("/webservices/dateUTC.php"), "", website, browseUrlCallback1);
      state=2;// Go to state to wait for response from browseURL

     timer = millis() + TIMEOUT_MS;// 5 second timeout
     break;
     case 2:
       if (millis() > timer)
       {
         // timeout waiting for response
         state=1;
         Serial.println("TIMOEOUT");
       }
       // waiting for response from previois calback
       ether.packetLoop(ether.packetReceive());
       break;
     case 3:
    
        while( ether.packetLoop(ether.packetReceive()));
         
        Serial.println("\nSending request for page /webservices/timeUTC.php");
        ether.browseUrl(PSTR("/webservices/timeUTC.php"), "", website, browseUrlCallback2);
        state = 4;
        timer = millis() + TIMEOUT_MS;// 5 second timeout
     break;
     case 4:
       if (millis() > timer)
       {
 
         // timeout waiting for response
         state=3;
         Serial.println("TIMOEOUT");
       }
       // waiting for response from previois calback
       ether.packetLoop(ether.packetReceive());
       break;
      case 5:
       // waiting for response from previois calback
        while( ether.packetLoop(ether.packetReceive()));
        state=0;
       break;
    default:
    break;
  }
}

*/
