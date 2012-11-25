/*
-------------How to wire Arduino to Smart meter using RJ11 cable-------------
Note: make sure to place a small resistor between between 5V & ground. Current from an Arduino output pin is 40mA, while 30mA is allowed on the P1 port.
Yellow to pin 9
Red to ground
Black to pin  4

-------------Example P1 telegram----------------------------
/ISk5\2ME382-1003

0-0:96.1.1(4B414C37303035303739393336333132)
1-0:1.8.1(00053.950*kWh)
1-0:1.8.2(00081.586*kWh)
1-0:2.8.1(00003.303*kWh)
1-0:2.8.2(00009.299*kWh)
0-0:96.14.0(0002)
1-0:1.7.0(0000.03*kW)
1-0:2.7.0(0000.00*kW)
0-0:17.0.0(0999.00*kW)
0-0:96.3.10(1)
0-0:96.13.1()
0-0:96.13.0()
!
*/

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,1,201);
IPAddress server(192,168,1,150);
EthernetClient client;

const int requestPin =  4;
int incomingByte = 0;
int pos181;
int pos182;
int pos281;
int pos282;
int P1_pos;
int P2_pos;
String inputString;
String T181;
String T182;
String T281;
String T282;
String P1;
String P2;
int counter = 0;

SoftwareSerial mySerial(9, 8, true); // RX, TX, inverted

void setup () {
  Serial.begin(9600);
  delay(1000);
  
  mySerial.begin(9600);
  delay(1000);

  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, HIGH);
}

void loop () {
  while (mySerial.available() > 0) {
    
    incomingByte = mySerial.read();
    incomingByte &= ~(1 << 7);    // forces 0th bit of x to be 0.  all other bits left alone.
    char inChar = (char)incomingByte;
    inputString += inChar; 
   }

   //If output from Smart meter is long enough, process it. Length needs to be checked individually just in case
   if (inputString.length() > 100) {
      //Serial.println(inputString);
      
      pos181 = inputString.indexOf("1-0:1.8.1", 0);
      T181 = inputString.substring(pos181 + 10, pos181 + 17);
      Serial.println("T181 = " + T181);
 
      pos182 = inputString.indexOf("1-0:1.8.2", pos181 + 1);
      T182 = inputString.substring(pos182 + 10, pos182 + 17);
      Serial.println("T182 = " + T182);
      
      pos281 = inputString.indexOf("1-0:2.8.1", pos182 + 1);
      T281 = inputString.substring(pos281 + 10, pos281 + 17);
      Serial.println("T281 = " + T281);
      
      pos282 = inputString.indexOf("1-0:2.8.2", pos281 + 1);
      T282 = inputString.substring(pos282 + 10, pos282 + 17);
      Serial.println("T282 = " + T282);
      
      P1_pos = inputString.indexOf("1-0:1.7.0", pos282 + 1);
      P1 = inputString.substring(P1_pos + 10, P1_pos + 17);
      Serial.println("P1 = " + P1);
      
      P2_pos = inputString.indexOf("1-0:2.7.0", P1_pos + 1);
      P2 = inputString.substring(P2_pos + 10, P2_pos + 17);
      Serial.println("P2 = " + P2);
      
      Serial.println(counter);
      
      httpRequest(); 
      inputString = "0";
      delay(1000);
      client.stop();
 
      if (counter < 6)
      {
        counter++;
      }
      else
      {
        counter = 0;
      }    
   }
}

void httpRequest() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
 
      if ( counter < 6 ) {
        client.println("GET /emoncms3/api/post?apikey=c2bc8720aa9e34cd9a661b1a7f400531&json={P1:" + P1 + ",P2:" + P2 + "} HTTP/1.1");
        Serial.println("GET /emoncms3/api/post?apikey=c2bc8720aa9e34cd9a661b1a7f400531&json={P1:" + P1 + ",P2:" + P2 + "} HTTP/1.1");
      }
      if ( counter > 5 ) {
        client.println("GET /emoncms3/api/post?apikey=c2bc8720aa9e34cd9a661b1a7f400531&json={T1:" + T181 + ",T2:" + T182 + ",T7:" + T281 + ",T8:" + T282 + ",P1:" + P1 + ",P2:" + P2 + "} HTTP/1.1");
        Serial.println("GET /emoncms3/api/post?apikey=c2bc8720aa9e34cd9a661b1a7f400531&json={T1:" + T181 + ",T2:" + T182 + ",T7:" + T281 + ",T8:" + T282 + ",P1:" + P1 + ",P2:" + P2 + "} HTTP/1.1");
      }
    client.println("Host: 192.168.1.150");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    Serial.println("Connection OK!");
  } 
  else {
    Serial.println("connection failed");
    client.stop();
  }
}
