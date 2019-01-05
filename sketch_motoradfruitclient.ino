#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <SPI.h>
#include <Ethernet.h>
Adafruit_MCP4725 dac;
//vars for ip/mac
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0x2A, 0xC7
};

//network variables
IPAddress ip(10, 7, 0, 180);
IPAddress myDns(10, 0, 0, 15);
IPAddress gateway(10, 7, 0, 20);
IPAddress subnet(255, 255, 255, 0);

//ints for pin numbers


int runpin = 3;
int groundpin = 2;
float delaytime;

String direction;
//varriables for reading
String readString; 
int currentspeed = 0;


// server port
EthernetServer server(23);
boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {
  //pin set up
  dac.begin(0x62);
  pinMode(runpin, OUTPUT);
  digitalWrite(runpin, LOW);
  
  
  // initialize the ethernet device
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
  
}

void loop() {
  
  //server start
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:

  if (client) {
    if (!alreadyConnected) {
      // clear out the input buffer:
      client.flush();
      client.println("connected");
      alreadyConnected = true;
    }
    
    if (client.available() > 0) {
      // read the bytes incoming from the client:
        char c = client.read();
       
       //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          if (c == '\n') {
            if (readString.equals("FORWARD")) {
              client.println("DF");
              direction = "CW";
            } else if (readString.equals("REVERSE")) {
              
              client.println("DR");
              direction = "CCW";

            } else  if (readString.equals("STOP")) {
              dac.setVoltage(2047.5, false);
              digitalWrite(runpin, LOW);
              client.println("DS");
            } else if (readString.equals("EMERGENCY")) {
              /*emergency pin*/
            } else if (readString.equals("RESET")) {
              /*Reset emergency*/
            } else if (readString.charAt(0) == 'S') {
               readString.remove(0, 1);
              
             if(readString.toInt() <= 0){
                dac.setVoltage(2047.5, false);
                digitalWrite(runpin, LOW);
                currentspeed = 0;
              } else {
                digitalWrite(runpin, HIGH);
                if(direction.equals("CCW")){
                  dac.setVoltage(double(abs(double(double(readString.toInt() * 0.0416)-2.5)) * 819), false);
                  currentspeed = (double(abs(double(double(readString.toInt() * 0.0416)-2.5)) * 819));

                  
                } else if (direction.equals("CW")) {
                  dac.setVoltage(double(double(double(readString.toInt() * 0.0416) + 2.5) * 819), false);
                  currentspeed = double(double(double(readString.toInt() * 0.0416) + 2.5) * 819);
                }
                
               
              }
              client.println(currentspeed);
              
            }else if (readString.equals("1")) {
              digitalWrite(runpin, LOW);
              client.println("connected");
            } 
            
          readString = "";
        } else {
          readString = readString + c; 
        }
       } 

        
     }
    }
  }
