#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YourWireless";
const char* password = "password";
const int PORT = 1234;
const char* hostName = "Desired_HostName";
int openCount = 0;
int closedCount = 0;
ESP8266WebServer server(PORT); // Server object creation and port assignment
String openCountStr = "N/A";
String closedCountStr = "N/A";

const String webPage = // changes to main webpage string can be added here
"<!doctype html>"
"<html>"
"<head>"
"  <script src=\"https://ajax.googleapis.com/ajax/libs/angularjs/1.5.6/angular.min.js\"></script>" // angular cdn
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"  <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">" // material cdn for css and design
"  <link rel=\"stylesheet\" href=\"https://code.getmdl.io/1.3.0/material.blue_grey-blue.min.css\" />"  // material cdn auto color styling
"  <script src=\"https://code.getmdl.io/1.3.0/material.min.js\"></script>" // material cdn
"  <title>IoHome Garage</title>"

// css
"  <style>"
    ".demo-list-action {"
      "min-width: inherit;"
    "}"
    "h2 {"
      "text-align: center;"
      "color: rgb(96,125,139);"
    "}"
    "h3 {"
      "text-align: center;"
      "color: rgb(96,125,139);"
    "}"
    ".material-icons {"
      "font-size: 35;"
    "}"
    "hr {"
      "display: block;"
      "height: 1px;"
      "border: 0;"
      "border-top: 5px solid rgb(96,125,139);"
      "margin: 1em 0;"
      "padding: 0;"
    "}"
    ".material-icons.md-light { color: rgba(255, 255, 255, 1); }"
    ".mdl-button--raised {"
      "min-width: 90%;"
      "font-weight: 550;"
    "}"
    ".demo-card-event.mdl-card {"
     " width: 100%;"
      "background: rgb(3,169,244);"
    "}"
    ".demo-card-event > .mdl-card__actions {"
      "background-color: rgb(96,125,139);"
    "}"
    ".demo-card-event > .mdl-card__title {"
      "align-items: flex-start;"
    "}"
    ".demo-card-event > .mdl-card__title > h4 {"
      "margin-top: 0;"
    "}"
    ".demo-card-event > .mdl-card__actions {"
      "display: flex;"
      "box-sizing:border-box;"
     " align-items: center;"
    "}"
    ".demo-card-event > .mdl-card__actions > .material-icons {"
      "padding-right: 10px;"
    "}"
    ".demo-card-event > .mdl-card__title,"
    ".demo-card-event > .mdl-card__actions,"
    ".demo-card-event > .mdl-card__actions > .mdl-button {"
      "background-color: rgb(96,125,139);"
"      color: #FFFFFF;"
    "}"
    ".mdl-spinner {"
      "margin-top: 8px;"
      "margin-right: 15px;"
    "}"
  "</style>"
"</head>"

"<div class=\"mdl-layout mdl-js-layout mdl-layout--fixed-header\">" // header for regular web page sizes
  "<header class=\"mdl-layout__header\">"
    "<div class=\"mdl-layout__header-row\">"
      "<i class=\"material-icons\">home</i>"
      "<span class=\"mdl-layout-title\"> IoHome Control </span>"
      "<div class=\"mdl-layout-spacer\"></div>"
      "<nav class=\"mdl-navigation mdl-layout--large-screen-only\">"
       " <a class=\"mdl-navigation__link\" href=\"http://cssoldier.blogspot.com/\">My Blog</a>"
        "<a class=\"mdl-navigation__link\" href=\"https://www.linkedin.com/in/robert-morris-575b89b0/\">Linked In</a>"
        "<a class=\"mdl-navigation__link\" href=\"https://plus.google.com/u/0/+RobertMorris-CSsoldier\">Google+</a>"
        "<a class=\"mdl-navigation__link\" href\"https://github.com/GettinDatFoShow\">GitHub</a>"
      "</nav>"
    "</div>"
  "</header>"

  "<div class=\"mdl-layout__drawer\">" // header for smaller web page sizes such as phones
    "<span class=\"mdl-layout-title\">Contact</span>"
    "<nav class=\"mdl-navigation\">"
      "<a class=\"mdl-navigation__link\" href=\"http://cssoldier.blogspot.com/\">My Blog</a>"
      "<a class=\"mdl-navigation__link\" href=\"https://www.linkedin.com/in/robert-morris-575b89b0/\">Linked In</a>"
      "<a class=\"mdl-navigation__link\" href=\"https://plus.google.com/u/0/+RobertMorris-CSsoldier\">Google+</a>"
      "<a class=\"mdl-navigation__link\" href=\"https://github.com/GettinDatFoShow\">GitHub</a>"
    "</nav>"
  "</div>"
  "<script>"
  "var app = angular.module('myApp',[]);" // important angularjs app declaration
  "app.controller('serverCtrl', ['$scope', '$http', function($scope, $http){" // important angular app declaration
    "$scope.CurrentDate = new Date();"
    "$scope.timeChecked = new Date();"
    "$scope.openClick = function(){" // on click function for garage door to open
      "$http.get('DoorOpen')" // http get request to server
      ".then(function(response) {" // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved
          "$scope.data = response.data;"
          "$scope.status = false;"
          "$scope.status = response.data.DoorData[0].status;"
          "$scope.timeChecked = new Date();"
          "$scope.doorStatus = $scope.status ? 'Open' : 'Close';"
       "});"
    "},"

    "$scope.closeClick = function() {" // on click function for garage door to close
      "$http.get('DoorClose')" // http get request to server
      ".then(function(response) {" // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved
          "$scope.data = response.data;"
          "$scope.status = true;"
          "$scope.status = response.data.DoorData[0].status;"
          "$scope.timeChecked = new Date();"
          "$scope.doorStatus = $scope.status ? 'Open' : 'Close';"
      "});"

    "},"

    "$scope.checkClick = function() {" // on click function for checking garage staus
      "$http.get('CheckDoor')" // http get request to server
      ".then(function(response) {" // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved
          "$scope.data = response.data;"
          "$scope.status = response.data.DoorData[0].status;"
          "$scope.timeChecked = new Date();"
          "$scope.doorStatus = $scope.status ? 'Open' : 'Close';"
      "});"

    "}"
    "(function() {" // immediately invoked function that is called when the page loads
"      $scope.checkClick;"
    "}());"

  "}]);"

  "</script>"
  "<main ng-app=\"myApp\" ng-controller=\"serverCtrl\" class=\"mdl-layout__content\">" // important to declare your ng app Controller
    "<div class=\"page-content\">"
      "<header>"
        "<span>"
        "<h2>"
          "Garage <br> <h3>{{CurrentDate | date:'dd, MMMM yyyy'}}</h3>" // date/time variable piped into angular date display
        "</h2>"
        "</span>"
        "<hr/>"
      "</header>"

      "<body >"

        "<div class=\"demo-list-action mdl-list\">"

          "<div ng-controller=\"serverCtrl\" class=\"mdl-list__item\">"
          " <span class=\"mdl-list__item-primary-content\">"
                "<a ng-click=\"openClick()\" id=\"show-snackbar\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">Open</a> "
                "<div id=\"snackbar-opening\" class=\"mdl-js-snackbar mdl-snackbar\">"
                  "<div class=\"mdl-snackbar__text\"></div>"
                  "<a class=\"mdl-snackbar__action\" type=\"button\"></a> "
                  "<div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>"
                "</div>"
                "</span>"
            "<a class=\"mdl-list__item-secondary-action\" ><i class=\"material-icons\">arrow_upward</i></a>"
          "</div>"

          "<div ng-controller=\"serverCtrl\" class=\"mdl-list__item\">"
           " <span  class=\"mdl-list__item-primary-content\">"
                "<a ng-click=\"closeClick()\" id=\"show-snackbar-2\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">CLOSE</a> "
               " <div id=\"snackbar-closing\" class=\"mdl-js-snackbar mdl-snackbar\">"
                  "<div class=\"mdl-snackbar__text\"></div>"
                 " <a class=\"mdl-snackbar__action\" type=\"button\"></a> "
                  "<div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>" // spinner status display when snackbar pops up
               " </div>"
            "</span>"
            "<a class=\"mdl-list__item-secondary-action\"><i class=\"material-icons\">arrow_downward</i></a>"
          "</div>"

          "<div ng-controller=\"serverCtrl\" class=\"mdl-list__item\">"
           " <span class=\"mdl-list__item-primary-content\">"
           "<a ng-click=\"checkClick()\" id=\"show-snackbar-3\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">CHECK</a> "
               " <div id=\"snackbar-checking\" class=\"mdl-js-snackbar mdl-snackbar\">"
                  "<div class=\"mdl-snackbar__text\"></div>"
                 " <a class=\"mdl-snackbar__action\" type=\"button\"></a> "
                  "<div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>"
               " </div>"
            "</span>"
            "<a class=\"mdl-list__item-secondary-action\"><i class=\"material-icons\">cached</i></a>"
          "</div>"
          "</div>" // CLOSES LIST ITEMS

"          <div ng-controller=\"serverCtrl\" class=\"demo-card-event mdl-card mdl-shadow--2dp\">" // demo card for information
"            <div class=\"mdl-card__title mdl-card--expand\">"
            "  <h4>"
            "<span>"
                "The Garage is Currently : "
                " {{ doorStatus }}" // if status true string is 'Open' else string is 'Closed'
                "<i ng-hide=\"status\"class=\"material-icons md-light\" >arrow_downward</i>" // ng hide show based on status boolean
                "<i ng-show=\"status \" class=\"material-icons md-light\" >arrow_downward</i>"
                "</span>"
                "<br>"
                " Last Time Checked : "
                "{{ timeChecked | date:'hh:mm:ss a'}} " // date/time variable piped into angular time display
              "</h4>"
            "</div>"
          "  <div class=\"mdl-card__actions mdl-card--border\">"
              "<a class=\"mdl-button mdl-button--colored mdl-js-button mdl-js-ripple-effect\">"
              "  Status Details"
"              </a>"
          "    <div class=\"mdl-layout-spacer\"></div>"
              "<i class=\"material-icons\">info</i>"
            "</div>"
          "</div>"

      "</body>"

    "</div>" // CLOSES DIV PAGE CONTENT ABOVE BODY
  "</main>"
  "</div>"
"<script>"

// immediately invoked function that is called when the page loads to create an event listener
"(function() {" // Demo snakbar function, provides a bottom of the page pop up alert
  "'use strict';"
  "var snackbarContainer = document.querySelector('#snackbar-checking');"
  "var showSnackbarButton = document.querySelector('#show-snackbar-3');"
  "var handler = function(event) {"
  "};"
  "showSnackbarButton.addEventListener('click', function() {"
   " 'use strict';"
    "var data = {"
      "message: 'No problem.....',"
     " timeout: 2000,"
      "actionHandler: handler,"
      "actionText: 'Checking!'"
    "};"
"      snackbarContainer.MaterialSnackbar.showSnackbar(data);"
"    });"
"  }());"
// immediately invoked function that is called when the page loads to create an event listener
"(function() {" // Demo snakbar function, provides a bottom of the page pop up alert
"'use strict';"
"var snackbarContainer = document.querySelector('#snackbar-closing');"
"var showSnackbarButton = document.querySelector('#show-snackbar-2');"
"var handler = function(event) {"
"};"
"showSnackbarButton.addEventListener('click', function() {"
" 'use strict';"
"var data = {"
"message: 'Your Garage door is ',"
" timeout: 12000,"
"actionHandler: handler,"
"actionText: 'Closing!'"
"};"
"      snackbarContainer.MaterialSnackbar.showSnackbar(data);"
"    });"
"  }());"
// immediately invoked function that is called when the page loads to create an event listener
"(function() {" // Demo snakbar function, provides a bottom of the page pop up alert
  "'use strict';"
  "var snackbarContainer = document.querySelector('#snackbar-opening');"
  "var showSnackbarButton = document.querySelector('#show-snackbar');"
  "var handler = function(event) {"
  "};"
  "showSnackbarButton.addEventListener('click', function() {"
    "'use strict';"
    "var data = {"
      "message: 'Your Garage door is ',"
      "timeout: 12000,"
      "actionHandler: handler,"
      "actionText: 'Opening!'"
    "};"
    "snackbarContainer.MaterialSnackbar.showSnackbar(data);"
  "});"
"}());"

"</script>"
"</html>";

IPAddress staticIP(192, 168, 1, 0); // enter your static ip here
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

int magCheck(){ // mag switch is set up on garage door to detect an open or closed door
  int count=0;
  int magValue=0;
  while ( count != 30 ){ // while loop to check magnetic switch
    count+=1;
    int read =digitalRead(MS_IN); // takes MS gpio pin value
    magValue += read;
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
    openCount=openCount+1; // count for opening door
    openCountStr = String(openCount);
  }
}

void doorClose()// function for garage door control to close door
{

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
    closedCount=closedCount+1; // count for closing door
    closedCountStr = String(closedCount);
  }else{

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
     server.send(200, "text/json", "");
     server.sendContent("{\"DoorData\": [ { \"status\": false, \"OpenCount\": \"" + openCountStr + "\", \"ClosedCount\": \"" + closedCountStr +"\" } ] }"); // response in json
     server.client().stop(); // Stop is needed because we sent no content length
 }else{ // open
     server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
     server.sendHeader("Pragma", "no-cache");
     server.sendHeader("Expires", "-1");
     server.setContentLength(CONTENT_LENGTH_UNKNOWN);
     server.send(200, "text/json", "");
     server.sendContent("{\"DoorData\": [ { \"status\": false, \"OpenCount\": \"" + openCountStr + "\", \"ClosedCount\": \"" + closedCountStr +"\" } ] }"); // response in json
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
    server.sendContent(webPage); // initial webpage load apon web request to your static ip and port
     server.client().stop(); // Stop is needed because we sent no content length
    });
  server.on("/DoorOpen", [](){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/json", "");
    server.sendContent("{\"DoorData\": [ { \"status\": false, \"OpenCount\": \"" + openCountStr + "\", \"ClosedCount\": \"" + closedCountStr +"\" } ] }"); // response in json
    server.client().stop(); // Stop is needed because we sent no content length
    doorOpen();
    });
  server.on("/DoorClose", [](){
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/json", "");
    server.sendContent("{\"DoorData\": [ { \"status\": false, \"OpenCount\": \"" + openCountStr + "\", \"ClosedCount\": \"" + closedCountStr +"\" } ] }"); // response in json
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
