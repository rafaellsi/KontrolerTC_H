#ifndef Ethernet_funk_h
#define Ethernet_funk_h



#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
//     #define RH_RF24_MAX_MESSAGE_LEN 28
    
//RH_NRF24 driver(NRF24_CE, NRF24_CSN);
//RHReliableDatagram manager(driver, CLIENT_ADDRESS);
  
//-----------------------------------------------------------------
void EthernetInit(boolean izpisShort) {
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, ETHER_CS_PIN) == 0) 
    Serial.println( "Failed to access Ethernet controller");
 
  if (!ether.staticSetup(myip, gwip, dnsip)) {
    Serial.println(F("Preveri povezavo ethernet modula!"));
  }  
  
  
    
  if (!izpisShort) {
    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip); 
    ether.printIp("DNS: ", ether.dnsip); 
  }
  else {
    Serial.print(F(" EthRein "));
  }  
  
  ether.parseIp(ether.hisip, "192.168.1.1");
//  Serial.println(ether.packetLoopIcmpCheckReply(gwip));	
//  ether.ntpRequest(ntpip, 8888); 
//  uint32_t *t;
//  Serial.println(ether.ntpProcessAnswer(t, 0)); 		

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
    "CO: $D</h3> <br>"),
      itime, 
      t0, t1, t2, t3, t4, t5, t6, t7, t8,
      rh,
      hum,
      coRawVal);
  return bfill.position();
}


//--------------------------------------------------------------------------------

  
  float sumPing = 0;
  unsigned long numPing = 0;

//--------------------------------------------------------------------------------

void CheckEthernet() {
 
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
 if (millis() - timerPing > 58750) {
   // ether.printIp("Pinging: ", ether.hisip);
    timerPing = millis();
    ether.clientIcmpRequest(ether.hisip);
    statusPing = false;
    do {
      len = ether.packetReceive();
      pos = ether.packetLoop(len);
      if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {  
        Serial.print(F(" Ep("));
        Serial.print(len);
        Serial.print(F(";"));
        Serial.print(pos);
        Serial.print(F(") "));
        sumPing += (millis() - timerPing); 
        numPing ++;
        statusPing = true;  
      } 
    }  while (statusPing == false && millis() - timerPing < 1000);
    if (statusPing == false) {
      EthernetInit(true);
      timerPing = millis();
      statusPing = true;
    }      
  }
  
  if (pos)  {  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
    Serial.print(F(" Ew("));
    Serial.print(len);
    Serial.print(F(";"));
    Serial.print(pos);
    Serial.print(F(") "));
  }
}  
  


#endif
