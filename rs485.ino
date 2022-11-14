/*

  --------------------------
  Created by Riccardo Bruno
  --------------------------

*/

//  Import Libraries
#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoModbus.h>
#include <ArduinoHttpClient.h>

//  Wi-Fi Credentials
const char ssid[] = "PELLICCIOTTI";          // PELLICCIOTTI Ciclarimpo_2.4GHz
const char pass[] = "SiccomeImmobile99"; // SiccomeImmobile99 Quelchenonpioveincielost4?
char server[] = "sparx99.it";
int port = 80;

int status = WL_IDLE_STATUS;

WiFiClient wifi;
HttpClient httpclient = HttpClient(wifi, server, port);

const unsigned long msdelay = 3600 * 1000 * 3; // DELAY TIME: 1h = 3600s = 3600000ms

void setup() {                                                            /////SETUP/////

  Serial.begin(9600); // pinMode (PinOutPWM, OUTPUT);

  while (status != WL_CONNECTED)
  {
    Serial.println("[?] Connecting to " + String(ssid) + "...");
    status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    delay(10000);
    Serial.println("[*] Connected to WiFi:");
    printWiFiStatus();
  }
  if (ModbusRTUClient.begin(2400)) { // start the Modbus RTU client
    Serial.println("[*] ModbusRTUClient started successully!");
  } else {
    Serial.println("[!] Failed starting ModbusRTUClient");
  }
}


void loop() {                                                             /////LOOP/////
  /*
    for (int i = 1; i < 7; i++) {
    float in = readActiveEnergy(i);
    float ex = readPassiveEnergy(i);
    httpRequest(i, in, ex);
    delay(2000);
    }
  */
  httpRequest(7, 0.01, 0.10);

  delay(msdelay);
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

float readActiveEnergy(int id)                                          /////readActiveEnergy()/////
{
  if (!ModbusRTUClient.requestFrom(id, INPUT_REGISTERS, 0x0048, 2))
  {
    Serial.println("[!] Failed reading Active Energy: ");
    Serial.println("ERROR FOR ID CLIENT:\t" + String(id) + ":");
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
{
  if (!ModbusRTUClient.requestFrom(id, INPUT_REGISTERS, 0x004A, 2))
  {
    Serial.print("[!] Failed to read Passive Energy");
    Serial.println("ERROR FOR ID CLIENT:\t" + String(id) + ":");
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

void httpRequest(int id, float in, float ex)                              /////httpRequest()/////
{
  if (!wifi.connect(server, port)) {
    Serial.println("[!] Failed connecting to sparx99.it");
  }
  else {
    Serial.println("[*] Connected to sparx99.it");

    /*
    // URL PHP
    String url = "GET /misure-energia/backModbus.php?";
    url += "ind=" + String(id);
    url += "&activeEn='" + String(in, 2) + "'";
    url += "&passiveEn='" + String(ex, 2) + "'";
    url += " HTTP/1.1";
    // URL PHP
    */

    //URL PHP
    String url = "GET /misure-energia/backModbus.php?ind=" + String(id) + 
    "&activeEn='" + String(in, 2) + 
    "'&passiveEn='" + String(ex, 2) + 
    "' HTTP/1.1";    
    //URL PHP

    httpclient.println(url);
    httpclient.println("Host: sparx99.it");
    httpclient.println("Connection: close");
    httpclient.println();

    Serial.println(httpclient.responseBody());
  }
}

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
