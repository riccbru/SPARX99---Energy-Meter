# SPARX99 - Energy-Meter

I realized this project for a customer's request who owns 6 apartments and wanted to monitor the power consumption.

So you will need:
1. Arduino MKR1000 with Wi-Fi Shield
2. SDM120CT Modbus, Eastron
3. HTTP Server on LAN

Firstly I built the Arduino board and downloaded the ArduinoModbus library from Arduino software.

I stuided the RS485 Modbus Protocol to understand and these devices store information, and then I started programming in Arduino: rs485.ino.
Then I configured the HTTP Web Server (Linux) to my company's url, and I moved all the php file into /var/www/html.
I didn't upload the index.php because it is private and you need to login with credentials to read your energy measures.
Arduino sends data through backModbus.php and the user can read them through index.php which is bundled with frames.php that prints all data in dat descending order.
