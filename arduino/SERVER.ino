//wifi libs
#include <SPI.h>
#include <WiFi101.h>

//motor libs
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

String motorState;

//network
char ssid[] = "tempest";
int status = WL_IDLE_STATUS;
WiFiServer server(80);


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);

void setup() {
  WiFi.setPins(8, 7, 4);
  Serial.begin(9600);      // initialize serial communication
  pinMode(9, OUTPUT);      // set the LED pin mode
  pinMode(3, OUTPUT);

  AFMS.begin();
  myMotor1->setSpeed(255);
  myMotor1->run(FORWARD);
  // turn on motor
  myMotor1->run(RELEASE);
  myMotor2->setSpeed(255);
  myMotor2->run(FORWARD);
  // turn on motor
  myMotor2->run(RELEASE);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWiFiStatus();                        // you're connected now, so print out the status
}


void loop() {
  uint8_t i;
  WiFiClient client = server.available();   // listen for incoming clients

  if (motorState.equals("Forward")) {
    myMotor1->run(FORWARD);
    myMotor1->setSpeed(255);
    myMotor2->run(FORWARD);
    myMotor2->setSpeed(255);
  }
  if (motorState.equals("Backward")) {
    myMotor1->run(BACKWARD);
    myMotor1->setSpeed(255);
    myMotor2->run(BACKWARD);
    myMotor2->setSpeed(255);
  }
  if (motorState.equals("Stop")) {
    myMotor1->run(BACKWARD);
    myMotor1->setSpeed(0);
    myMotor2->run(BACKWARD);
    myMotor2->setSpeed(0);
  }

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a><br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(9, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(9, LOW);                // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /stop")) {
          motorState = "Stop";
        }
        if (currentLine.endsWith("GET /forward")) {
          motorState = "Forward";
        }
        if (currentLine.endsWith("GET /backward")) {
          motorState = "Backward";
        }
      }
    }
    // close the connection:
    // client.stop();
    //Serial.println("client disonnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
