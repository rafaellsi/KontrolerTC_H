/*
| Post temp. values from a DS18B20 to ThingSpeak using the Ethercard interface based on the 
| ENC28J60 chip.
| Based on the Ethercard example from www.jeelabs.org
| Phil Grant Jan 2014
*/
#include <EtherCard.h>
// change these settings to match your own setup
#define APIKEY  "xxxxxxxxxxxxxxxx"

//DHT Setup
#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)  
DHT dht(DHTPIN, DHTTYPE);

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0xXX,0xXX,0xXX,0xXX,0xXX,0xXX };

char website[] PROGMEM = "api.thingspeak.com";

byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;

void setup () {
  Serial.begin(57600);
    dht.begin(); //Initializate DHT
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
    delay(2000);  // Wait a few seconds between measurements.
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity(); // Read temperature as Celsius (the default)
  char h_buffer[10];
  String Hstring = dtostrf(h,0,5,h_buffer); //Convert Humidity value to String

  float t = dht.readTemperature(); // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);
  char t_buffer[10];
  String Tstring = dtostrf(t,0,5,t_buffer); //Convert Temperature value to String

    byte sd = stash.create();
    stash.print("field1=");
    stash.println(Hstring);
    stash.print("&field2=");
    stash.println(Tstring);
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