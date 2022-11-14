/*

  --------------------------
  Created by Riccardo Bruno
  --------------------------

*/

// CREARE ROUTINE PER NTP Req: se match orario -> read()&httpRequest()

//  Import Libraries
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <ArduinoModbus.h>

//  Wi-Fi Credentials
const char ssid[] = "PELLICCIOTTI";
const char pass[] = "SiccomeImmobile99"; 
char server[] = "sparx99.it/misure-energia/andreassi"; //misure-energia.sparx99.it/andreassi

int status = WL_IDLE_STATUS;

WiFiClient client;

unsigned long rate = 3600000;        // default 1000 refresh rate in ms, misurare 2 volte/gg (12h = 43200000ms, 1h = 3600000 ms)
unsigned long lastMillis = 0;
unsigned long lastConnectionTime = 0; // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L;

WiFiUDP Udp;
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

String strUrl = "sparx99.it/misure-energia/andreassi"; // misure-energia.sparx99.it/andreassi

// float potenzaResistenza = 3.0; // kW
// int PinOutPWM = 3;
float exKWH;

bool sendReq = false;

float toFloat(String s)                                                 /////toFloat()/////
  {
    union
    {
      uint32_t i;
      float f;
    } data;
    int str_len = s.length() + 1;
    char modbus_data[str_len];
    s.toCharArray(modbus_data, str_len);
    data.i = strtoul(modbus_data, NULL, 16);
    return data.f;
  }

void printWiFiStatus()                                                  /////printWiFiStatus()/////
  {
    Serial.print("\tSSID:\t"); // print the SSID of the network you're attached to
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP(); // print your WiFi shield's IP address
    Serial.print("\tIP:\t");
    Serial.println(ip);
    long rssi = WiFi.RSSI(); // print the received signal strength:
    Serial.print("\tRSSI:\t");
    Serial.print(rssi);
    Serial.println(" dBm");
  }

float readActiveEnergy(int ind)                                       /////readActiveEnergy()/////
  { 
    if (!ModbusRTUClient.requestFrom(ind, INPUT_REGISTERS, 0x0048, 2))
    { // make the call to the register
      Serial.print("[!] Failed reading Active Energy: ");
      Serial.println(ModbusRTUClient.lastError()); // error handler
    }
    else
    {
      Serial.println("[*] Successfully read Active Energy!");
      uint16_t word1 = ModbusRTUClient.read();
      uint16_t word2 = ModbusRTUClient.read();
      String hex1 = String(word1, HEX);
      String hex2 = String(word2, HEX);
      String hexTot = hex1 + hex2;
      return toFloat(hexTot);
    }
    return -1;
  }

float readPassiveEnergy(int id)                                          /////readPassiveEnergy()/////
  { // PassiveEnergy
    if (!ModbusRTUClient.requestFrom(id, INPUT_REGISTERS, 0x004A, 2))
    { // make the call to the register
      Serial.print("[!] Failed to read Passive Energy: ");
      Serial.println(ModbusRTUClient.lastError()); // error handler
    }
    else
    {
      Serial.println("[*] Successfully read Passive Energy!");
      uint16_t word1 = ModbusRTUClient.read();
      uint16_t word2 = ModbusRTUClient.read();
      String hex1 = String(word1, HEX);
      String hex2 = String(word2, HEX);
      String hexTot = hex1 + hex2;
      return toFloat(hexTot);
    }
    return -1;
  }

void httpRequest(int id, float in, float ex) {                          /////httpRequest()/////
  
    client.stop(); // close any connection before send a new request. This will free the socket on the WiFi shield
    if (client.connect(server, 80)) {
      Serial.println("[*] CONNECTED TO SERVER!");

      String strURL = "";
      
      // URL PHP
      strURL = "GET /backModbus.php?ind=" + String(id) + "&activeEn='" + String(in, 2) + "'&passiveEn='" + String(ex, 2) + "'";
      // URL PHP

      client.println(strURL);
      Serial.println(strURL);

      client.println(strUrl);
      Serial.println(strUrl);
      lastConnectionTime = millis();
    }
    else {
      Serial.println("[!] Failed connecting to sparx99.it"); // if you couldn't make a connection
    }
  }


unsigned long sendNTPpacket(IPAddress& address)                         /////sendNTPpacket()/////
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode

  packetBuffer[1] = 0;     // Stratum, or type of clock

  packetBuffer[2] = 6;     // Polling Interval

  packetBuffer[3] = 0xEC;  // Peer Clock Precision

  // 8 bytes of zero for Root Delay & Root Dispersion

  packetBuffer[12]  = 49;

  packetBuffer[13]  = 0x4E;

  packetBuffer[14]  = 49;

  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now

  // you can send a packet requesting a timestamp:

  Udp.beginPacket(address, 123); //NTP requests are to port 123

  Udp.write(packetBuffer, NTP_PACKET_SIZE);

  Udp.endPacket();

}

void setup() {                                                            /////SETUP/////
  
  Serial.begin(9600); // pinMode (PinOutPWM, OUTPUT);
  
  while (!Serial)
    ;
  while (status != WL_CONNECTED)
  {
    Serial.println("[?] Connecting to " + String(ssid) + "...");
    status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    delay(10000);
    Serial.println("[*] Connected to WiFi:");
    printWiFiStatus();
  }
  if (!ModbusRTUClient.begin(2400))
  { // start the Modbus RTU client
    Serial.println("[!] Failed starting ModbusRTUClient: ");
    while (1)
      ;
  }
  else
  {
    Serial.println("[*] ModbusRTUClient started successfully!");
  }
}

void loop() {                                                             /////LOOP/////

  while (client.available()) {

    char c = client.read();

    Serial.write(c);

    sendReq = true;
  }
  
  if (millis() - lastMillis > rate) { // lastMillis = 0; rate = 3600000                       
      
    lastMillis = millis();

    sendNTPpacket(timeServer);

    if (Udp.parsePacket()) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      if ( (((epoch  % 86400L) / 3600) + 2) == 3 || (((epoch  % 86400L) / 3600) + 2) == 20) { // se sono le 3 am o le 18 pm
        // for i in [1, 6]: read values & make http request to sparx99.it
        for (int id = 1; id < 7; id++) {
          float inKWH = readActiveEnergy(id); // ActiveEnergy
          delay(100);
          float exKWH = readPassiveEnergy(id); // PassiveEnergy
          delay(100);
          
          if (millis() - lastConnectionTime > postingInterval && send) {
            httpRequest(id, inKWH, exKWH);
            sendReq = false;
          }
        }      } else {
        Serial.println("[!] Non è il momento di leggere valori!");
      }
    }
}
    
    
  }  
