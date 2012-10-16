/*
-------------How to wire Arduino to Smart meter using RJ11 cable-------------
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
int T1_pos;
int T2_pos;
int T7_pos;
int T8_pos;
int P1_pos;
int P2_pos;
String inputString;
String T1;
String T2;
String T7;
String T8;
String P1;
String P2;
String outputString;

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
      
      T1_pos = inputString.indexOf("1-0:1.8.1", 0);
      T1 = inputString.substring(T1_pos + 10, T1_pos + 19);
      Serial.println("T1 = " + T1);
 
      T2_pos = inputString.indexOf("1-0:1.8.2", T1_pos + 1);
      T2 = inputString.substring(T2_pos + 10, T2_pos + 19);
      Serial.println("T2 = " + T2);
      
      T7_pos = inputString.indexOf("1-0:2.8.1", T2_pos + 1);
      T7 = inputString.substring(T7_pos + 10, T7_pos + 19);
      Serial.println("T7 = " + T7);
      
      T8_pos = inputString.indexOf("1-0:2.8.2", T7_pos + 1);
      T8 = inputString.substring(T8_pos + 10, T8_pos + 19);
      Serial.println("T8 = " + T8);
      
      P1_pos = inputString.indexOf("1-0:1.7.0", T8_pos + 1);
      P1 = inputString.substring(P1_pos + 10, P1_pos + 17);
      Serial.println("P1 = " + P1);
      
      P2_pos = inputString.indexOf("1-0:2.7.0", P1_pos + 1);
      P2 = inputString.substring(P2_pos + 10, P2_pos + 17);
      Serial.println("P2 = " + P2);
      
      outputString = "GET /emoncms3/api/post?apikey=c2bc8720aa9e34cd9a661b1a7f400531&json={T1:" + T1 + ",T2:" + T2 + ",T7:" + T7 + ",T8:" + T8 + ",P1:" + P1 + ",P2:" + P2 + "} HTTP/1.1";
      httpRequest(); 
      inputString = "0";
      delay(1000);
      client.stop();
   }
}

void httpRequest() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    client.println(outputString);
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
