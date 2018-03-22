#include <dht.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
dht DHT;
#define DHT11_PIN A0
// defines pins numbers
const int trigPin = 6;
const int echoPin = 7;
// defines variables
long duration;
int distance;
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output  
  pinMode(echoPin, INPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  
int chk = DHT.read11(DHT11_PIN);
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance= duration*0.034/2;
Serial.print("Rain Measured: ");
Serial.print(8-distance);
Serial.print("mm");
Serial.print("\ttemperature= ");
Serial.print(DHT.temperature);
Serial.print("\thumidity= ");
Serial.println(DHT.humidity);
delay(1000);
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
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
          client.println("<font color='#006699' bgcolor='#9f9fdf'><h1><br>Temperature:<br><br>");
          client.println("<input type=text value=&deg;C style='width: 300px; height: 50px; font-size: 30px; text-align:center;' readonly><br>");
          client.println(DHT.temperature);
          client.println("<br><hr>");
          client.println("Humidity:<br><br>");
          client.println("<input type=text value='%' style='width: 300px; height: 50px; font-size: 30px; text-align:center; ' readonly></font></h1>");
          client.println(DHT.humidity);
          client.println("Rain Measured:<br><br>");
          client.println("<input type=text value='%' style='width: 300px; height: 50px; font-size: 30px; text-align:center; ' readonly></font></h1>");
          client.println(8-distance);
          // output the value of each analog input pin
         
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

