#ifndef Ethernet_UIP_funk_h
#define Ethernet_UIP_funk_h

#define DEBUG_ETH

#include "ThingSpeak.h"

void CheckEthernet(void);
void ThingSpeakUpdate(void);


unsigned long myChannelNumber = 57866;
const char * myWriteAPIKey = "CB9RI4WF29AG0QF1";

IPAddress myIP(192,168,178,50);
byte myMAC[] = {0x74,0x69,0x69,0x2D,0x30,0x31};

EthernetClient clientTh;
EthernetServer server(80);
//--------------------------------------------------------------------------------
void EthernetInit(boolean izpisShort) {

//  Ethernet.begin(mac);
//  Ethernet.begin(mac,IPAddress(192,168,178,50));
  Ethernet.begin(myMAC, myIP);
  server.begin();
  
  EthernetIzpisInfo();
}

//--------------------------------------------------------------------------------
void CheckEthernet() {
unsigned long timeout = 5000;
unsigned long connectStart = millis();

 EthernetClient client = server.available();
  if (client) {
#ifdef DEBUG_ETH
    Serial.println("");
    Serial.println("new client: ");
    Serial.println(client);
    // an http request ends with a blank line
#endif
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        connectStart = millis();
        char c = client.read();
//            Serial.write(c);
#ifdef DEBUG_ETH
        Serial.print(c);
#endif        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if ((c == '\n' && currentLineIsBlank)) {
//            Serial.print("n\\ n+");
            // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 59");  // refresh the page automatically every 5 sec
//"<meta http-equiv='refresh' content='59'/>"
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          char itime[20];
          sprintf(itime, "%02d.%02d.%d %02d:%02d:%02d", day(), month(), year(), hour(), minute(), second());
          client.println(itime);

            // output the value of each analog input pin
          for (int i = 0; i < 9; i++) {
            client.print("T");
            client.print(i+1);
            client.print(": ");
            client.print(cTemperatura[i]);
            client.println("<br />");
         }
          client.print("State: ");
          client.print(prevTCState);
          client.print(!releState_ventKompTC);
          client.print(!releState_kompTC);
          client.print(prevCrpTCState);
          client.print(prevVentTCState);
          client.print(prevCrpRadState);
          client.print(!stateCevStPecTC);
          client.println("</html>");


           break;
        }
        if (c == '\n') {         
          // you're starting a new line
          currentLineIsBlank = true;
//          Serial.print("n\\ n");
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
 //         Serial.print("r\\ r");
        }
      }
      if(millis() - connectStart > timeout) {
        client.stop();
        Ethernet.maintain(); 
        return;  
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
#ifdef DEBUG_ETH   
    Serial.println("client disconnected");
#endif 
   
  }

/*
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
 */
}


void ThingSpeakUpdate(void) {

  ThingSpeak.begin(clientTh);
  //ThingSpeak.begin(clientTh);
  ThingSpeak.setField(1,cTemperatura[0]);
  ThingSpeak.setField(2,cTemperatura[CRPALKA_0]);
  ThingSpeak.setField(3,cTemperatura[PEC_PV]);
  ThingSpeak.setField(4,cTemperatura[PEC_DV]);
  ThingSpeak.setField(5,cTemperatura[RAD_PV]);
  ThingSpeak.setField(6,cTemperatura[RAD_DV]);
  ThingSpeak.setField(7,cTemperatura[PEC_TC_DV]);
  ThingSpeak.setField(8,cTemperatura[OKOLICA_0]);
  
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
#ifdef DEBUG_ETH
  Serial.print(" Things: ");
  Serial.print(myChannelNumber);
  Serial.print(" ");
  Serial.print(myWriteAPIKey);
#endif  
}


void EthernetIzpisInfo(void) {
  Serial.println("UIPEthernet ");
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  
}
#endif
