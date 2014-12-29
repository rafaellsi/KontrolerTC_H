#ifndef Ethernet_funk_h
#define Ethernet_funk_h

void EthernetInit(void) {
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, ETHER_CS_PIN) == 0) 
    Serial.println( "Failed to access Ethernet controller");
 
  if (!ether.staticSetup(myip, gwip, dnsip)) {
    Serial.println(F("Preveri povezavo ethernet modula!"));
  }  
  
  
    
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip); 
  
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
  dtostrf(cTemperatura[0], 2, 2, t0);
  dtostrf(cTemperatura[1], 2, 2, t1);
  dtostrf(cTemperatura[2], 2, 2, t2);
  dtostrf(cTemperatura[3], 2, 2, t3);
  dtostrf(cTemperatura[4], 2, 2, t4);
  dtostrf(cTemperatura[5], 2, 2, t5);
  dtostrf(cTemperatura[6], 2, 2, t6);
  
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
    "<h3> T0: $SC <br> T1: $SC <br> T2: $SC <br> T3: $SC <br> T4: $SC <br> T5: $SC <br> T6: $SC</h3>"),
      itime, 
      t0, t1, t2, t3, t4, t5, t6);
  return bfill.position();
}
//--------------------------------------------------------------------------------




#endif
