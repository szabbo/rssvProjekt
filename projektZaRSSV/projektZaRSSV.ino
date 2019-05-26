/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi

 */

#include <SPI.h>
#include <Ethernet.h>

//neki novi senzor include
#include "DHT.h"
#define DHTPIN A0 // na koji pin je povezan DHT11
#define DHTTYPE DHT11
#define outputBUZ 3 // Buzzer pin

//PIR senzor
int inPin = 9; // signal s HC-SR501 na pinu9
int led1 = 8; // ledica na pinu8
int led2 = 7; // ledica na pinu7
int led3 = 6; // ledica na pinu6
boolean ocitanje;
boolean alarm = false;

//sonar
int trig = 5; // Trig na PIN5
int echo = 4; // Echo na PIN4

//neki novi senzor
DHT dht(DHTPIN, DHTTYPE);

//najnoviji senzor
byte photoR = 2; // analog 0 is the photoresistor
int base; // the base value, read only once
byte threshold = 100; // minimum difference

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 21, 77);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

      //pir senzor i ledice
    pinMode(inPin, INPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);

    //sonar
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    Serial.begin(9600); // zapocinjem serijsku komunikaciju
    Serial.println("Mjerenje udaljenost s HC-SR04 senzorom");
    Serial.println("");

    //neki novi senzor
    Serial.begin(9600);
    Serial.println("DHT11 test!");
    dht.begin();

    //buzz lightning
    pinMode(outputBUZ, OUTPUT);

    //najnoviji senzor
    pinMode (photoR, INPUT); // The light reading
    Serial.begin(9600); // starting serial comm
    base = analogRead(photoR); // read the initial value

    
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

// Beep function fo the buzzer
void beep(byte delayms, int pin){
 analogWrite(pin, 120);
 delay(delayms);
 analogWrite(pin, 0);
 delay(delayms);
}

//void lopovLopova(){
// EthernetClient client = server.available();
//  client.println("HTTP/1.1 200 OK");
//          client.println("Content-Type: text/html");
//          client.println("Connection: close");  // the connection will be closed after completion of the response
//          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
//          client.println();
//          client.println("<!DOCTYPE HTML>");
//          client.println("<html>");
//
//  client.print("LOPOV!<br />");
//  client.print("NEMA NIKOGA!");
//          
//          client.println("</html>");
//
//}


void loop() {
  //pir ledice
    // Read data from the sensor:
   int ocitanje = digitalRead (inPin);
  
   // If movement is detected turn on the LED
   // and beep the buzzer
   if(ocitanje == HIGH){
     digitalWrite(led1, HIGH);
     digitalWrite(led2, HIGH);
     digitalWrite(led3, HIGH);
     //lopovLopova();
     alarm = true;
     //buzz lightning
    beep(100, outputBUZ); 
   }else if(ocitanje == LOW){
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    delay(500);
    }

    //sonar
    // buduci da ne koristimo library, dolje je napisan nacin komunikacije s modulom
    long vrijeme, udaljenost;
    digitalWrite(trig, HIGH); // Slanje triga
    delayMicroseconds(10); // Odaslani trig traje 10 μs = 0.1 ms
    digitalWrite(trig, LOW);
    vrijeme = pulseIn(echo, HIGH); // Primanje reflektiranog triga
    udaljenost = (vrijeme/2) / 28; // Racunanje udaljenosti, u centimetrima
    // konacna udaljenost senzora od predmeta
    // u cm je varijabla "udaljenost"
    Serial.print(udaljenost);
    Serial.println(" cm");
    delay(50); // preporucena pauza izmedu ocitanja ne bi trebala biti manja od 50ms

    //neki novi senzor
    //pričekati par sekundi između svakog mjerenja
    delay(2000);
    // očitava vlagu u varijabli h te temperaturu u varijabli t
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Neuspješno očitavanje senzora! ");
      //ODKOMENTIRAJ KASNIJE
      return;
    }
    Serial.print("Vlaga: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C ");

    //najnoviji sezor
    int light = analogRead(photoR); // determine light level
    if (light < base - threshold) // check if dark
     {
     Serial.print("Mrak je! Ocitao: "); // Output a message
     Serial.println(light); // to the user

     digitalWrite(led1, HIGH);
     digitalWrite(led2, HIGH);
     digitalWrite(led3, HIGH);
     //buzz lightning
      beep(100, outputBUZ); 
     }else{
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      //delay(500);
     }
     Serial.println(light); // If not dark, print value
     //delay(500);


    
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
//          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//            int sensorReading = analogRead(analogChannel);
//            client.print("analog input ");
//            client.print(analogChannel);
//            client.print(" is ");
//            client.print(sensorReading);
//            client.println("<br />");
//          }
          //lopovLopova();
          if(alarm == true)
          {
            client.print("Houston, we have a problem!<br />");
          }else{
            client.print("Nema lopova!");
          }
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }

  
}
