/*
| Post temp. values from a DS18B20 to ThingSpeak using the Ethercard interface based on the 
| ENC28J60 chip.
| Based on the Ethercard example from www.jeelabs.org
| Phil Grant Jan 2014
*/
#include <EtherCard.h>
// change these settings to match your own setup
#define APIKEY  "xxxxxxxxxxxxxxxx"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h> //DS18B20 support
// =========== Setup the Temperature sensor details =================
#define ONE_WIRE_BUS 7  // DS18B20 Temperature chip i/o on pin D7
#define TEMPERATURE_PRECISION 9
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Insert the ID of your temp sensor here, for the sketch, visit here
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
DeviceAddress Thermometer = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX }; // probe
  
  
// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0xXX,0xXX,0xXX,0xXX,0xXX,0xXX };

char website[] PROGMEM = "api.thingspeak.com";

byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;

void setup () {
  Serial.begin(57600);
  Serial.println("\n[webClient]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer) {
    timer = millis() + 60000; //Transmit every minute
    sensors.requestTemperatures();
    delay(200);
    // generate two fake values as payload - by using a separate stash,
    // we can determine the size of the generated message ahead of time
    byte sd = stash.create();
    stash.print("field1=");
    stash.println(String(sensors.getTempC(Thermometer),DEC));
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
            website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();
  }
}
