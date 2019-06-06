#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#define echoPin 9 //calculatedcalculatedDistanceSensor pin
#define triggerPin 8 //calculatedcalculatedDistanceSensor pin
#define buzzer 7 // Buzzer pin
#define inputPIR 6 // MotionSensor input pin
#define ledGreen 5 // Green Led
#define ledYellow 4 // Yellow Led
#define ledRed 3 // Red Led
#define photoResistor  A1 //svijetlo
#define DHTsensorPin A0 //vlaga

/******************** ETHERNET SETTINGS ********************/
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x85, 0xD9 };   //physical mac address
byte ip[] = { 192, 168, 1, 3 };                     //ip in lan
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
byte gateway[] = { 192, 168, 0, 1 };                   //default gateway
EthernetServer server(80);                             //server port

/******************** DISTANCE SETTINGS ********************/
long duration;
long calculatedDistance;

/******************** LIGHT SETTINGS ********************/
int baseLightValue;
byte threshold = 100;

/******************** DHT SETTINGS ********************/
DHT DHTsensor (DHTsensorPin, DHT11);

void setup() {
    Ethernet.begin(mac,ip,gateway,subnet);     // initialize Ethernet device
    server.begin();                            // start to listen for clients

    pinMode(echoPin, INPUT);
    pinMode(inputPIR, INPUT);
    pinMode(photoResistor, INPUT);

    /******************** OUTPUT SETTINGS ********************/
    pinMode(triggerPin, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledRed, OUTPUT);

    baseLightValue = analogRead(photoResistor);

    Serial.begin(9600); 
}

void loop() {
  int temperature = DHTsensor.readTemperature();
  Serial.println(temperature);
  
  int pirValue = digitalRead(inputPIR);

  int lightValue = analogRead(photoResistor);
  
  digitalWrite(triggerPin, LOW);
  delay(1);
  digitalWrite(triggerPin, HIGH);
  delay(5);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  calculatedDistance = (duration / 2) / 28;

  
  EthernetClient client = server.available();    // look for the client
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();

  client.println("<!DOCTYPE html>");      //web page is made using html
  client.println("<html>");
  client.println("<head>");
  client.println("<title>PROTUPROVALNI SUSTAV</title>");
  client.println("<meta http-equiv=\"refresh\" content=\"1\">");
  
  client.println("</head>");
  client.println("<body>");
  client.println("<h1> PROTUPROVALNI SUSTAV </h1>");

  if(calculatedDistance < 4 || pirValue == HIGH || (lightValue < baseLightValue - threshold) || temperature > 30){
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, HIGH);
    beep(100);
    client.println("<h2>ALARM IS ON</h2>");
  }
  else{
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, LOW);
    client.println("<h2>ALARM IS OFF</h2>");
  }
  client.println("</body>");
  client.println("</html>");
  client.stop();
  delay(500);
}

// Beep function fo the buzzer
void beep(byte delayms){
  digitalWrite(buzzer, 500);
  delay(delayms);
  digitalWrite(buzzer, LOW);
  delay(delayms);
}
