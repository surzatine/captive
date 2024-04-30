#include <EEPROM.h>

#define STRING_ADDRESS 0      // Starting address for string in EEPROM
#define MAX_STRING_LENGTH 32  // Maximum allowed string length

char myString[MAX_STRING_LENGTH];

#define LED_BUILTIN 2
#include <WiFi.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(8, 8, 4, 4);  // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);
WiFiServer ssidName(8081);

String responseHTML = ""
                      ""
                      "<!DOCTYPE html>\n"
                      "<html>\n"
                      "<body onload=\"myFunction()\">\n"
                      "<script>\n"
                      "function myFunction() {\n"
                      "  location.replace(\"http://8.8.4.1:8080/index.html\")\n"
                      "}\n"
                      "</script>\n"
                      "</body>\n"
                      "</html> \n"
                      "";



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // EEPROM Begin
  // EEPROM.begin(50);

  // Initialize EEPROM with the maximum string length
  if (!EEPROM.begin(MAX_STRING_LENGTH)) {
    Serial.println("Failed to initialize EEPROM");
    while (1)
      ;  // Halt if EEPROM initialization fails
  }

  // Read existing string from EEPROM (optional)
  readStringFromEEPROM();




  // char ssid[] = "Wifi SSID";


  // WiFi.mode(WIFI_AP);
  // WiFi.softAP(ssid);
  // Serial.println(ssid);
  // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // // if DNSServer is started with "*" for domain name, it will reply with
  // // provided IP to all DNS request
  // dnsServer.start(DNS_PORT, "*", apIP);

  // server.begin();
  // ssidName.begin();
  // put your setup code here, to run once:
}

void loop() {

  pinMode(LED_BUILTIN, OUTPUT);

  //digitalWrite(LED_BUILTIN, HIGH);
  // put your main code here, to run repeatedly:
  dnsServer.processNextRequest();
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }


  WiFiClient ssidClient = ssidName.available();  // listen for incoming clients

  if (ssidClient) {                   // if you get a client,
    Serial.println("New Client.");    // print a message out the serial port
    String currentLine = "";          // make a String to hold incoming data from the client
    while (ssidClient.connected()) {  // loop while the client's connected
      if (ssidClient.available()) {   // if there's bytes to read from the client,
        char c = ssidClient.read();   // read a byte, then
        // Serial.write(c);              // print it out the serial monitor
        if (c == '\n') {  // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            ssidClient.println("HTTP/1.1 200 OK");
            ssidClient.println("Content-type:text/html");
            ssidClient.println();

            // the content of the HTTP response follows the header:
            ssidClient.print(" <a href=\"/\">Refresh</a> <br>");
            ssidClient.print("<center><form><input type='text' name='ssid' placeholder='Enter SSID' height='40px' width='40%' required> <br><br>");
            ssidClient.print("<input type='submit' name='update' value='UPDATE' height='40px' width='40%'> </form><br><br></center>");

            ssidClient.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            ssidClient.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            ssidClient.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        // Serial.println(currentLine);
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
          Serial.println(currentLine);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
          Serial.println(currentLine);
        }
        if (currentLine.endsWith("GET /Reset")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
          Serial.println(currentLine);
        }
        if (currentLine.endsWith("&update=UPDATE")) {
          String s = currentLine;
          // s.replace("http://8.8.4.4:8081/?ssid=", "");
          s.replace("GET /?ssid=", "");
          s.replace("&update=UPDATE", "");
          s.replace("+", " ");




          String sentence = s;
          
          Serial.println(sentence.length());


          if(sentence.length() <  MAX_STRING_LENGTH - 1){
            for (int i = sentence.length(); i < (MAX_STRING_LENGTH -1); i++) {
              sentence.concat(" ");
            }
          }

          // Example string to write
          String dataToWrite = sentence;



          // Check if string length exceeds limit
          if (dataToWrite.length() > MAX_STRING_LENGTH - 1) {
            Serial.println("String too long for EEPROM!");
            return;
          }

          // Convert string to null-terminated character array
          dataToWrite.toCharArray(myString, MAX_STRING_LENGTH);

          

          // Write string to EEPROM character by character
          for (int i = 0; i < dataToWrite.length(); i++) {
            EEPROM.write(STRING_ADDRESS + i, myString[i]);
          }

          // Commit the write operation (important!)
          EEPROM.commit();
          Serial.println("String written to EEPROM");
          delay(5000);

          // Read string from EEPROM and print it
          readStringFromEEPROM();
          delay(5000);

          Serial.println("[*]");
          Serial.println(sentence);
          // EEPROM.writeString(address, sentence);
        }
      }
    }
    // close the connection:
    ssidClient.stop();
    Serial.println("Client Disconnected.");
  }
}


// AP Setup
void readStringFromEEPROM() {
  char tempString[MAX_STRING_LENGTH];

  // Read characters from EEPROM
  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    tempString[i] = EEPROM.read(STRING_ADDRESS + i);

    // Check for null terminator (end of string)
    if (tempString[i] == '\0') {
      break;
    }
  }

  // Print the retrieved string
  Serial.print("String read from EEPROM: ");
  Serial.println(tempString);

  // Wifi AP
  String ssid;
  if (tempString == "") {
    ssid = "Wifi SSID";
  } else {
    ssid = tempString;
  }


  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  Serial.println(ssid);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
  ssidName.begin();
}
