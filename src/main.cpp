// Created and written by Robert Morris, USAR OIF Veteran and CS Student at Delaware State University
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <Time.h> // Time.h is incorrect lib name

// WiFi Connections Details
const char* ssid = "YourWiFiNameHere";
const char* password = "**********";
const int PORT = 1000; /// choose your port here
const char* hostName = "HostNameYouChoose";

ESP8266WebServer server(PORT); // Server object creation and port assignment.

String webPage =        "<head>" // changes to main webpage string can be added here
         "<title>IoHome Garage</title>"
         "<style>"
         ".main body {"
         "background-color: #ffffff;"
         "font-family: Helvetica Neue;"
             "font-size: 14px;"
             "align-content: center;"
             "align-self: center;"
             "align-items: center;"
           "}"
           ".main {"
             "flex: 1;"
             "display: flex;"
             "flex-direction: column;"
             "height: 100vh;"
             "background-color: #ffffff;"
             "font-family: Helvetica Neue;"
             "font-size: 14px;"
             "align-content: center;"
             "align-self: center;"
           "}"
           "div{"
             "align-content: center;"
             "align-self: center;"
             "align-items: center;"
           "}"
           ".btn {"
             "border-radius: 4px;"
             "background: #0899d4;"
             "color: #ffffff;"
             "text-transform: uppercase;"
             "border: none;"
             "padding: 10px 20px;"
             "letter-spacing: 0.02em;"
             "font-weight: 700;"
             "text-align: center;"
             "min-width: 100%;"
             "box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);"
             "border: 2px solid #8a8a8a;"
           "}"
           ".btn:hover {"
             "cursor: pointer;"
             "background: #8a8a8a;"
             "color: #0899d4;"
             "border: 2px solid #0899d4;"
           "}"
           ".statusUnknown {"
             "border-radius: 4px;"
             "background: #8a8a8a;"
             "color: #ffffff;"
             "text-transform: uppercase;"
             "padding: 10px 20px;"
             "letter-spacing: 0.02em;"
             "font-weight: 700;"
             "text-align: center;"
             "min-width: 100%;"
             "box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);"
             "border: 2px solid #8a8a8a;"
           "}"
           ".statusOpen{"
             "border-radius: 4px;"
             "background: #3cab26;"
             "color: #ffffff;"
             "text-transform: uppercase;"
             "padding: 10px 20px;"
             "letter-spacing: 0.02em;"
             "font-weight: 700;"
             "text-align: center;"
             "min-width: 100%;"
             "box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);"
             "border: 2px solid #8a8a8a;"
           "}"
           ".statusClosed{"
             "border-radius: 4px;"
             "background: #e32424;"
             "color: #ffffff;"
             "text-transform: uppercase;"
             "padding: 10px 20px;"
             "letter-spacing: 0.02em;"
             "font-weight: 700;"
             "text-align: center;"
             "min-width: 100%;"
             "box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);"
             "border: 2px solid #8a8a8a;"
           "}"
           "h1 {"
             "position: relative;"
             "text-align: center;"
             "margin: 4px 0px;"
             "padding: 0px 30px;"
             "font-size: 3em;"
             "color: #0899d4;"
             "text-shadow:  2px 2px 4px #8a8a8a;"
           "}"
           "h2{"
             "position: relative;"
             "text-align: center;"
             "margin: 4px 0px;"
             "padding: 0px 30px;"
             "font-size: 2.0em;"
             "color: #0899d4;"
             "text-shadow:  0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);"
             "text-shadow:  1px 1px 2px #8a8a8a;"
           "}"
           "hr {"
             "background: #0899d4;"
           "}"
           ".main ul {"
             "position: relative;"
             "width: 60vh;"
             "align-content: center;"
             "align-self: center;"
             "align-items: center;"
           "}"

           ".main li{"
             "position: relative;"
           "}"
         "</style>"
       "</head>"
             "<header>"
               "<h1>"
                   "IoHome Garage"
               "</h1>"
               "<hr/>"
             "</header>"
             "<body class=\"main\">"
               "<ul>"
                 "<li>"
                   "<a href=\"DoorOpen\"><button class=\"btn\">Open</button></a>&nbsp;"
                 "</li>"
                 "<li>"
                   "<a href=\"DoorClose\"><button class=\"btn\">Close</button></a>&nbsp;"
                 "</li>"
                 "<li>"
                   "<a href=\"CheckDoor\"><button class=\"btn\">Check</button></a>&nbsp;"
                 "</li>"
               "</ul>"
             "</body>"
             "<div>"
               "<hr/>"
               "<h2>"
                 "Garage Status:";
 String open = "<ul><li><button class=\"statusOpen\">Open</button></li></ul></h2></div>"; // webpage ending for open button
 String closed = "<ul><li><button class=\"statusClosed\">Closed</button></li></ul></h2></div>"; // webpage ending for close button
 String unknown = "<ul><li><button class=\"statusUnknown\">Unverified</button></li></ul></h2></div>"; // webpage ending for unknown status.

IPAddress staticIP(192, 168, 1, 69); // enter your static ip here
IPAddress gateway(192, 168, 1 ,1); // enter your gateway
IPAddress subnet(255, 255, 255, 0); // enter your subnet mask

// GPIO definition Details
const uint8_t DOOR = 5; // define gpio out for open closing switch
const uint8_t MS_IN = 10; // MagSitch in Read (High Closed) (Low Open)
const uint8_t LED = 16; // LED indicator

// boolean check variables (0 or 1)

uint8_t isOpen = 0; // system door variables
uint8_t magSwitch = 0; // magnetic switch variable
uint8_t light = 0;

// Hash code variables here for sercurity (later)

// Setup ------------------------------------------------------------

int magCheck(){
  int count=0;
  int magValue=0;
  while ( count != 30 ){ // while loop to check magnetic switch
    count+=1;
    int read =digitalRead(MS_IN); // takes MS gpio pin value
    magValue += read;
    Serial.print(magValue);
  }
  if(magValue < (count/2)){
    isOpen = 0;
  }
  else{
    isOpen = 1;
  }
  return isOpen;
}

void doorOpen() // function for garage door control to open door
{
  isOpen = magCheck();

  if(isOpen == 1){
    // DOOR IS ALREADY OPEN
  }else{
    digitalWrite(DOOR, 1);
    delay(1000);
    digitalWrite(DOOR, 0);
    uint8_t count = 0;
    while(count != 5){
      digitalWrite(LED, 1);
      delay(200);
      digitalWrite(LED, 0);
      delay(200);
      count+=1;
    }
  }
}

void doorClose()// function for garage door control to close door
{
  // checkDoor();
  isOpen = magCheck();
  if(isOpen == 1){
    digitalWrite(DOOR, 1);
    delay(1000);
    digitalWrite(DOOR, 0);
    uint8_t count = 0;

    while(count != 5){
      digitalWrite(LED, 1);
      delay(200);
      digitalWrite(LED, 0);
      delay(200);
      count+=1;
    }
  }else{
    // door is already closed
  }
}

void checkDoor()
{ // takes MS gpio pin value
 isOpen = magCheck();

 if(isOpen == 0){
     server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
     server.sendHeader("Pragma", "no-cache");
     server.sendHeader("Expires", "-1");
     server.setContentLength(CONTENT_LENGTH_UNKNOWN);
     server.send(200, "text/html", "");
     server.sendContent(webPage + closed);
    server.client().stop(); // Stop is needed because we sent no content length
 }else{ // open
     server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
     server.sendHeader("Pragma", "no-cache");
     server.sendHeader("Expires", "-1");
     server.setContentLength(CONTENT_LENGTH_UNKNOWN);
     server.send(200, "text/html", "");
     server.sendContent(webPage + open);
     server.client().stop(); // Stop is needed because we sent no content length
   }
}

void setup()
{
  pinMode(MS_IN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(DOOR, OUTPUT);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  digitalWrite(LED, LOW);
  light = 0;
  while (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(LED, !light);
      delay(500);
      Serial.print(".");
      light = !light;
    }
  digitalWrite(LED, LOW);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.hostname(hostName);
  Serial.println("");
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway is: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet is: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("HostName is : ");
  Serial.println(WiFi.hostname());
  Serial.println("this is the magCheck");
  Serial.println(isOpen);
  server.on("/", [](){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(webPage + unknown);
    server.client().stop(); // Stop is needed because we sent no content length
    });
  server.on("/DoorOpen", [](){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(webPage + open);
      server.client().stop(); // Stop is needed because we sent no content length
      doorOpen();
    });
  server.on("/DoorClose", [](){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(webPage + closed);
      server.client().stop(); // Stop is needed because we sent no content length
      doorClose();
    });
  server.on("/CheckDoor", checkDoor);
  server.begin();
}

void loop()
{

  server.handleClient();

}
