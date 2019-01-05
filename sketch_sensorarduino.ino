#include <SPI.h>
#include <Ethernet.h>
#define encoder0PinA  2
#define encoder0PinB  7
#define encoder0PinZ  5
#define calibrationA  A0



byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x2F
};

//network variables
IPAddress ip(10, 7, 0, 190);
IPAddress myDns(10, 0, 0, 15);
IPAddress gateway(10, 7, 0, 20);
IPAddress subnet(255, 255, 255, 0);

volatile long encoder0Pos=0;
volatile long encoderZPos=0;
long newposition;
long oldposition = 0;
long newtime;
long oldtime = 0;
long velnew;
long velold;
float newangle;
int result;
float oldangle;
String currentdirection;
String olddirection;
String encoderDirection;
boolean calibrationtest = false;
float calibrationAvoltage;


EthernetServer server(23);
boolean alreadyConnected = false; 
float calibrationstate = 0;
String readString;

void setup() {
  pinMode(encoder0PinA, INPUT);
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor
  pinMode(encoder0PinB, INPUT);
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor
  attachInterrupt(0, doEncoder, RISING);  // encoDER ON PIN 2
  pinMode(calibrationA, INPUT);

  // initialize the ethernet device
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
}

void loop() {
  //server start
  EthernetClient client = server.available();
  
  if (client) {
    if (!alreadyConnected) {
      client.flush();
      client.println("connected");
      alreadyConnected = true;
    }
    newangle = round(((float)encoder0Pos / (float)100) * (float)3.76); 
    newposition = (long)newangle;

    if (currentdirection != olddirection){
      client.print("D");
      client.println(currentdirection);
    }
      
     
      //if new position is different then old send new angel
      if (newangle != oldangle) {
        client.print("A");
        client.println(round(newangle), DEC);
  
      }

  
    newtime = millis();
    velnew = abs((newposition-oldposition)* 1000/(newtime-oldtime));
    
    if (velold != velnew){
      client.print ("S");
      client.println (velnew);
    }
  
  oldposition = newposition;
  olddirection = currentdirection;
  oldtime = newtime;
  oldangle = newangle;
  velold = velnew;
  
  delay(250);   
  }
}

void doEncoder() {
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos--;
    currentdirection = "CW";
  } else {
    encoder0Pos++;
    currentdirection = "CCW";
  }
  //Serial.println(encoder0Pos, DEC);
}

void doEncoderZ() {
  if (encoderDirection != currentDirection) {
    encoderDirection = currentDirection;
  } else {
    encoder0Pos;
  }
}
int roundto45(int number) {
  if (number > 0){ 
    result = number + 45/2;
    result = result - (result % 45);
  } else if (number < 0){
    result = number - 45/2;
    result = result + (abs(result) % 45);
  } else{
    result = 0;
  }
  return result;
}

