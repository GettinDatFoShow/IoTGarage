#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleTimer.h>


const char* ssid = "YourWireless";
const char* password = "password";
const int PORT = 1234;
const char* hostName = "Desired_HostName";

ESP8266WebServer server(PORT); // Server object creation and port assignment
SimpleTimer timer;
String openCountStr = "N/A";
String closedCountStr = "N/A";
s
IPAddress staticIP(192, 168, 1, 0); // enter your static ip here
IPAddress gateway(192, 168, 1 ,1); // enter your gateway
IPAddress subnet(255, 255, 255, 0); // enter your subnet mask

// GPIO definition Details
const uint8_t DOOR = 5; // define gpio out for open closing switch
const uint8_t MS_IN = 10; // MagSitch in Read (High Closed) (Low Open)
const uint8_t LED = 16; // LED indicator
bool isOpen = 0; // system door variables
bool doorStatus = false;
bool timerState = false;
bool timerSwitch = true;
int openCount = 0;
int closedCount = 0;
int magSwitch = 0; // magnetic switch variable
uint8_t light = 0;
uint16_t minutes = 15;
long currentTimer = minutes*60000;

// Hash code variables here for sercurity (later)


// function declarations
bool magCheck();
void doorClose();
void doorOpen();
void checkDoor();
void timedDoorClose();
void timerEnable();
void timerDisable();
void doorTimer();
void intitializeSite();
void respond();

const uint8_t timerId = timer.setTimer(minutes,  timedDoorClose, 1);

// ========================== INTIAL WEBPAGE ======================================================================

const String webPage =
"  <!doctype html>\n"
" <html ng-app=\"myApp\" ng-controller=\"serverCtrl\">\n"
" <head>\n"
"   <!-- // angular cdn -->\n"
"   <script src=\"https://ajax.googleapis.com/ajax/libs/angularjs/1.6.6/angular.min.js\"></script>\n"
"   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"   <!-- // material cdn for css and design -->\n"
"   <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">\n"
"    <!-- // material cdn auto color styling -->\n"
"   <link rel=\"stylesheet\" href=\"https://code.getmdl.io/1.3.0/material.blue_grey-blue.min.css\" />\n"
"    <!-- // material cdn -->\n"
"   <script src=\"https://code.getmdl.io/1.3.0/material.min.js\"></script>\n"
"   <script src=\"https://cdnjs.cloudflare.com/ajax/libs/moment.js/2.18.1/moment.min.js\"></script>\n"
"   <title>IoHome</title>\n"
"\n"
"<!-- // css -->\n"
"   <style>\n"
"     .demo-list-action {\n"
"       min-width: inherit;\n"
"     }\n"
"     h2 {\n"
"       text-align: center;\n"
"       color: rgb(96,125,139);\n"
"     }\n"
"     h3 {\n"
"       text-align: center;\n"
"       color: rgb(96,125,139);\n"
"     }\n"
"     h4  {\n"
"       text-align: center;\n"
"       color: rgb(96,125,139);\n"
"     }\n"
"     .data-card{\n"
"       color: rgb(255,255,255);\n"
"     }\n"
"     a.data-card{\n"
"       color: rgb(255,255,255);\n"
"     }\n"
"     .material-icons {\n"
"       font-size: 30;\n"
"     }\n"
"     .mdl-list__item .mdl-list__item-primary-content {\n"
"       padding-left: 40px;\n"
"       padding-right: 40px;\n"
"     }\n"
"     hr {\n"
"\n"
"       display: block;\n"
"       height: 1px;\n"
"       border: 0;\n"
"       border-top: 5px solid rgb(96,125,139);\n"
"       margin: 1em 0;\n"
"       padding: 0;\n"
"     }\n"
"     .mdl-card {\n"
"       padding-left: 40px;\n"
"       padding-right: 40px;\n"
"     }\n"
"     .material-icons.md-light { color: rgba(255, 255, 255, 1); }\n"
"     .mdl-button--raised {\n"
"       min-width: 90%;\n"
"       font-weight: 550;\n"
"     }\n"
"     .demo-card-event.mdl-card {\n"
"       width: 100%;\n"
"     }\n"
"     .demo-card-event > .mdl-card__actions {\n"
"       background-color: rgb(96,125,139);\n"
"     }\n"
"     .demo-card-event > .mdl-card__title {\n"
"       align-items: flex-start;\n"
"     }\n"
"     .demo-card-event > .mdl-card__title > h4 {\n"
"       margin-top: 0;\n"
"     }\n"
"     .demo-card-event > .mdl-card__actions {\n"
"       display: flex;\n"
"       box-sizing:border-box;\n"
"       align-items: center;\n"
"     }\n"
"     .demo-card-event > .mdl-card__actions > .material-icons {\n"
"       padding-right: 10px;\n"
"     }\n"
"     .demo-card-event > .mdl-card__title,\n"
"     .demo-card-event > .mdl-card__actions,\n"
"     .demo-card-event > .mdl-card__actions > .mdl-button {\n"
"       background-color: rgb(96,125,139);\n"
"       color: #FFFFFF;\n"
"     }\n"
"     .mdl-spinner {\n"
"       margin-top: 8px;\n"
"       margin-right: 15px;\n"
"     }\n"
"   </style>\n"
" </head>\n"
"\n"
"<!-- // header for regular web page sizes -->\n"
" <div class=\"mdl-layout mdl-js-layout mdl-layout--fixed-header\">\n"
"   <header class=\"mdl-layout__header\">\n"
"     <div class=\"mdl-layout__header-row\">\n"
"       <i class=\"material-icons\">home</i>\n"
"       <span class=\"mdl-layout-title\"> IoHome Control - {{ deviceName }} </span>\n"
"       <div class=\"mdl-layout-spacer\"></div>\n"
"       <nav class=\"mdl-navigation mdl-layout--large-screen-only\">\n"
"         <a class=\"mdl-navigation__link\" href=\"http://cssoldier.blogspot.com/\">My Blog</a>\n"
"         <a class=\"mdl-navigation__link\" href=\"https://www.linkedin.com/in/robert-morris-575b89b0/\">Linked In</a>\n"
"         <a class=\"mdl-navigation__link\" href=\"https://plus.google.com/u/0/+RobertMorris-CSsoldier\">Google+</a>\n"
"         <a class=\"mdl-navigation__link\" href\"https://github.com/GettinDatFoShow\">GitHub</a>\n"
"       </nav>\n"
"     </div>\n"
"   </header>\n"
"<!-- // header for smaller web page sizes such as phones -->\n"
"   <div class=\"mdl-layout__drawer\">\n"
"     <span class=\"mdl-layout-title\">Contact</span>\n"
"     <nav class=\"mdl-navigation\">\n"
"       <a class=\"mdl-navigation__link\" href=\"http://cssoldier.blogspot.com/\">My Blog</a>\n"
"       <a class=\"mdl-navigation__link\" href=\"https://www.linkedin.com/in/robert-morris-575b89b0/\">Linked In</a>\n"
"       <a class=\"mdl-navigation__link\" href=\"https://plus.google.com/u/0/+RobertMorris-CSsoldier\">Google+</a>\n"
"       <a class=\"mdl-navigation__link\" href=\"https://github.com/GettinDatFoShow\">GitHub</a>\n"
"     </nav>\n"
"   </div>\n"
"<!-- // important to declare your ng app Controller -->\n"
"   <main ng-controller=\"serverCtrl\" class=\"mdl-layout__content\">\n"
"     <div  class=\"page-content\">\n"
"       <header>\n"
"         <span>\n"
"         <h4>{{ CurrentDate | date:'dd, MMMM yyyy'}}</h4>\n"
"         </span>\n"
"         <hr/>\n"
"       </header>\n"
"\n"
"       <body >\n"
"\n"
"         <div class=\"demo-list-action mdl-list\">\n"
"\n"
"           <div class=\"mdl-list__item\">\n"
"            <span class=\"mdl-list__item-primary-content\">\n"
"                 <md-button ng-click=\"openClick()\" id=\"show-snackbar\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">Open\n"
"                   <a class=\"mdl-layout-spacer\"></a>\n"
"                <i class=\"material-icons\" style=\"color:rgb(255,255,255)\">arrow_upward</i>\n"
"                 </md-button>\n"
"                 <div id=\"snackbar-opening\" class=\"mdl-js-snackbar mdl-snackbar\">\n"
"                   <div class=\"mdl-snackbar__text\"></div>\n"
"                   <a class=\"mdl-snackbar__action\" type=\"button\"></a>\n"
"                   <div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>\n"
"                 </div>\n"
"           </span>\n"
"           </div>\n"
"\n"
"          <div class=\"mdl-list__item\">\n"
"             <span  class=\"mdl-list__item-primary-content\">\n"
"                 <md-button ng-click=\"closeClick()\" id=\"show-snackbar-2\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">CLOSE\n"
"                   <a class=\"mdl-layout-spacer\"></a>\n"
"                   <i class=\"material-icons\" style=\"color:rgb(255,255,255)\">arrow_downward</i>\n"
"                 </md-button>\n"
"\n"
"                 <div id=\"snackbar-closing\" class=\"mdl-js-snackbar mdl-snackbar\">\n"
"                   <div class=\"mdl-snackbar__text\"></div>\n"
"                   <a class=\"mdl-snackbar__action\" type=\"button\"></a>\n"
"                   <!-- // spinner status display when snackbar pops up -->\n"
"                   <div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>\n"
"                 </div>\n"
"           </span>\n"
"           </div>\n"
"\n"
"           <div class=\"mdl-list__item\">\n"
"             <span class=\"mdl-list__item-primary-content\">\n"
"            <md-button ng-click=\"checkClick()\" id=\"show-snackbar-3\" class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect mdl-button--accent\" type=\"button\">CHECK\n"
"              <a class=\"mdl-layout-spacer\"></a>\n"
"              <i class=\"material-icons\" style=\"color:rgb(255,255,255)\">cached</i>\n"
"            </md-button>\n"
"                 <div id=\"snackbar-checking\" class=\"mdl-js-snackbar mdl-snackbar\">\n"
"                   <div class=\"mdl-snackbar__text\"></div>\n"
"                   <a class=\"mdl-snackbar__action\" type=\"button\"></a>\n"
"                   <div class=\"mdl-spinner mdl-spinner--single-color mdl-js-spinner is-active\"></div>\n"
"                 </div>\n"
"           </span>\n"
"           </div>\n"
"           </div>\n"
"\n"
"            <!-- // demo card for information -->\n"
"           <div class=\"demo-card-event mdl-card mdl-shadow--2dp\">\n"
"             <div  class=\"mdl-card__actions\">\n"
"                <!-- // if status true string is 'Open' else string is 'Closed' -->\n"
"                  <a class=\"data-card\"> The Garage is\n"
"                    <a class=\"data-card\" ng-show=\"status\"> : Open </a>\n"
"                    <a class=\"data-card\" ng-hide=\"status\" > : Closed </a>\n"
"                  </a>\n"
"                   <a class=\"mdl-layout-spacer\"></a>\n"
"                   <md-button ng-show=\"status\" ng-click=\"closeClick()\" class=\"material-icons md-warn\" style=\"color:#ff6700\"> ic_warning </md-button>\n"
"                   <md-button ng-hide=\"status\" ng-click=\"openClick()\" class=\"material-icons\" > arrow_downward </md-button>\n"
"                 </div>\n"
"                 <div class=\"mdl-card__actions \">\n"
"                     <!-- // date/time variable piped into angular time display -->\n"
"                       <a class=\"data-card\"> Last Checked: {{ timeChecked | date:'hh:mm a'}} </a>\n"
"                       <a class=\"mdl-layout-spacer\"></a>\n"
"                       <md-button class=\"material-icons\">event </md-button>\n"
"                   </div>\n"
"                   <div  class=\"mdl-card__actions \">\n"
"                     <!-- here is the displayed timer minutes -->\n"
"                       <a class=\"data-card\" >Timer\n"
"                       <a class=\"data-card\" ng-show=\"timerState\">: '{{ minutes }}' Min </a>\n"
"                       <a class=\"data-card\" ng-hide=\"timerState\" >: Timer Off <a> </span>\n"
"                       <a class=\"mdl-layout-spacer\"></a>\n"
"                       <md-button ng-click=\"disableClick()\" ng-show=\"timerState\" class=\"material-icons\" > ic_access_time </md-button>\n"
"                       <md-button ng-click=\"enableClick()\" ng-hide=\"timerState\" class=\"material-icons\" style=\"color:#ff6700\"> ic_warning </md-button>\n"
"                  </div>\n"
"\n"
"\n"
"            <div  class=\"mdl-card__actions mdl-card--border\">\n"
"              <a ng-show=\"timerState\" ng-click=\"disableClick()\" class=\"mdl-button mdl-button--colored mdl-js-button mdl-js-ripple-effect\">\n"
"                Timer On\n"
"              </a>\n"
"              <a ng-hide=\"timerState\" ng-click=\"enableClick()\" class=\"mdl-button mdl-button--colored mdl-js-button mdl-js-ripple-effect\">\n"
"                Timer Off\n"
"              </a>\n"
"              <div class=\"mdl-layout-spacer\"></div>\n"
"              <!-- // ng hide show based on status boolean -->\n"
"              <i ng-show=\"timerState\" class=\"material-icons md-light\" ng-click=\"disableClick()\" >ic_timer</i>\n"
"              <i ng-hide=\"timerState \" class=\"material-icons md-light\" ng-click=\"enableClick()\" >ic_timer_off</i>\n"
"            </div>\n"
"          </div>\n"
"      </body>\n"
"    </div>\n"
"  </main>\n"
"  </div>\n"
"\n"
"\n"
"<!-- // ************************ JAVA SCRIPT ***************************************************** -->\n"
"\n"
"  <script>\n"
"  var app = angular.module('myApp',[]) // important angularjs app declaration\n"
"  .controller('serverCtrl', ['$scope', '$http', function($scope, $http){ // important angular app declaration\n"
"    $scope.CurrentDate = new Date();\n"
"    $scope.timeChecked = new Date();\n"
"    $scope.timerState = 'Unknown';\n"
"    $scope.minutes = 0;\n"
"    $scope.deviceName = 'Garage';\n"
"    // $scope.timer = $moment.duration(10, 'seconds').timer({loop: true}, function() {\n"
"    //   $scope.checkClick;\n"
"    // });\n"
"    $scope.openClick = function(){ // on click function for garage door to open\n"
"      $http.get('DoorOpen') // http get request to server\n"
"      .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"          $scope.data = response.data;\n"
"          $scope.status = response.data.DoorData.status;\n"
"          $scope.timeChecked = new Date();\n"
"          $scope.timerState = response.data.DoorData.timerState;\n"
"          console.log('response : ' + response);\n"
"          console.log('status : ' + $scope.status);\n"
"          console.log('TimerState : ' + $scope.timerState);\n"
"          $scope.minutes = response.data.DoorData.timerMinutes;\n"
"       });\n"
"    },\n"
"\n"
"    $scope.closeClick = function() { // on click function for garage door to close\n"
"      $http.get('DoorClose') // http get request to server\n"
"      .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"          $scope.data = response.data;\n"
"          $scope.status = response.data.DoorData.status;\n"
"          $scope.timeChecked = new Date();\n"
"          $scope.timerState = response.data.DoorData.timerState;\n"
"          console.log('response : ' + response);\n"
"          console.log('status : ' + $scope.status);\n"
"          console.log('TimerState : ' + $scope.timerState);\n"
"          $scope.minutes = response.data.DoorData.timerMinutes;\n"
"      });\n"
"    },\n"
"\n"
"    $scope.checkClick = function() { // on click function for checking garage staus\n"
"      $http.get('CheckDoor') // http get request to server\n"
"      .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"          $scope.data = response.data;\n"
"          $scope.status = response.data.DoorData.status;\n"
"          $scope.timeChecked = new Date();\n"
"          $scope.timerState = response.data.DoorData.timerState;\n"
"          console.log('response : ' + response);\n"
"          console.log('status : ' + $scope.status);\n"
"          console.log('TimerState : ' + $scope.timerState);\n"
"          $scope.minutes = response.data.DoorData.timerMinutes;\n"
"          console.log('timerMinutes : ' + $scope.minutes);\n"
"      });\n"
"    },\n"
"\n"
"      $scope.disableClick = function() { // on click function for checking garage staus\n"
"        $http.get('DisableTimer') // http get request to server\n"
"        .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"            $scope.data = response.data;\n"
"            $scope.status = response.data.DoorData.status;\n"
"            $scope.timerState = response.data.DoorData.timerState;\n"
"            $scope.minutes = response.data.DoorData.timerMinutes;\n"
"            console.log('response : ' + response);\n"
"            console.log('status : ' + $scope.status);\n"
"            console.log('TimerState : ' + $scope.timerState);\n"
"        });\n"
"    },\n"
"\n"
"        $scope.enableClick = function() { // on click function for checking garage staus\n"
"          $http.get('EnableTimer') // http get request to server\n"
"          .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"              $scope.data = response.data;\n"
"              $scope.status = response.data.DoorData.status;\n"
"              $scope.timerState = response.data.DoorData.timerState;\n"
"              $scope.minutes = response.data.DoorData.timerMinutes;\n"
"              console.log('response : ' + response);\n"
"              console.log('status : ' + $scope.status);\n"
"              console.log('TimerState : ' + $scope.timerState);\n"
"          });\n"
"    },\n"
"\n"
"    (function() { // immediately invoked function that is called when the page loads\n"
"      $http.get('CheckDoor') // http get request to server\n"
"      .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"          $scope.data = response.data;\n"
"          $scope.status = response.data.DoorData.status;\n"
"          $scope.timeChecked = new Date();\n"
"          $scope.timerState = response.data.DoorData.timerState;\n"
"          console.log('response : ' + response);\n"
"          console.log('status : ' + $scope.status);\n"
"          console.log('TimerState : ' + $scope.timerState);\n"
"          $scope.minutes = response.data.DoorData.timerMinutes;\n"
"          console.log('timerMinutes : ' + $scope.minutes);\n"
"      });\n"
"    }());\n"
"    $interval(function() {\n // timer to call back and check on the door in 30 seconds"
"      $http.get('CheckDoor') // http get request to server\n"
"      .then(function(response) { // creating a promise.. keeps the page from waiting for a response but acts after a response is recieved\n"
"          $scope.data = response.data;\n"
"          $scope.status = response.data.DoorData.status;\n"
"          $scope.timeChecked = new Date();\n"
"          $scope.timerState = response.data.DoorData.timerState;\n"
"          console.log('response : ' + response);\n"
"          console.log('status : ' + $scope.status);\n"
"          console.log('TimerState : ' + $scope.timerState);\n"
"          $scope.minutes = response.data.DoorData.timerMinutes;\n"
"          console.log('timerMinutes : ' + $scope.minutes);\n"
"      });\n"
"    }, 30000);\n"
"  }]);"
"\n"
"  }]);\n"
"\n"
"// immediately invoked function that is called when the page loads to create an event listener\n"
"(function() { // Demo snakbar function, provides a bottom of the page pop up alert\n"
"  'use strict';\n"
"  var snackbarContainer = document.querySelector('#snackbar-checking');\n"
"  var showSnackbarButton = document.querySelector('#show-snackbar-3');\n"
"  var handler = function(event) {\n"
"  };\n"
"  showSnackbarButton.addEventListener('click', function() {\n"
"    'use strict';\n"
"    var data = {\n"
"      message: 'No problem.....',\n"
"      timeout: 2000,\n"
"      actionHandler: handler,\n"
"      actionText: 'Checking!'\n"
"    };\n"
"      snackbarContainer.MaterialSnackbar.showSnackbar(data);\n"
"    });\n"
"  }());\n"
"// immediately invoked function that is called when the page loads to create an event listener\n"
"(function() { // Demo snakbar function, provides a bottom of the page pop up alert\n"
"'use strict';\n"
"var snackbarContainer = document.querySelector('#snackbar-closing');\n"
"var showSnackbarButton = document.querySelector('#show-snackbar-2');\n"
"var handler = function(event) {\n"
"};\n"
"showSnackbarButton.addEventListener('click', function() {\n"
" 'use strict';\n"
"var data = {\n"
"message: 'Your Garage door is ',\n"
" timeout: 12000,\n"
"actionHandler: handler,\n"
"actionText: 'Closing!'\n"
"};\n"
"      snackbarContainer.MaterialSnackbar.showSnackbar(data);\n"
"    });\n"
"  }());\n"
"// immediately invoked function that is called when the page loads to create an event listener\n"
"(function() { // Demo snakbar function, provides a bottom of the page pop up alert\n"
"  'use strict';\n"
"  var snackbarContainer = document.querySelector('#snackbar-opening');\n"
"  var showSnackbarButton = document.querySelector('#show-snackbar');\n"
"  var handler = function(event) {\n"
"  };\n"
"  showSnackbarButton.addEventListener('click', function() {\n"
"    'use strict';\n"
"    var data = {\n"
"      message: 'Your Garage door is ',\n"
"      timeout: 12000,\n"
"      actionHandler: handler,\n"
"      actionText: 'Opening!'\n"
"    };\n"
"    snackbarContainer.MaterialSnackbar.showSnackbar(data);\n"
"  });\n"
"}());\n"
"\n"
"</script>\n"
"</html>\n"
"";

// Setup ------------------------------------------------------------

bool magCheck() {
  uint8_t count=0;
  uint8_t magValue=0;
  uint8_t read=0;
  while ( count != 30 ){ // while loop to check magnetic switch
    count+=1;
    read=digitalRead(MS_IN); // takes MS gpio pin value
    magValue += read;
  }
  if(magValue < (count/2)){
    isOpen = false;
    doorStatus = false;
  }
  else{
    isOpen = true;
    doorStatus = true;
  }
  return isOpen;
}

void doorOpen() // function for garage door control to open door
{
  isOpen = magCheck();

  if(isOpen){
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
    doorTimer();
    openCount=openCount+1;
    openCountStr = String(openCount);
  }
  respond();
}

void doorClose()// function for garage door control to close door
{
  isOpen = magCheck();
  if(isOpen){
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
    closedCount=closedCount+1;
    closedCountStr = String(closedCount);
  }else{
      // door is already closed
  }
  respond();
}


void checkDoor() // function to check if door is open or closed.
{ // takes MS gpio pin value
  isOpen = magCheck();
  respond();
}

// timer setup for callback function to close door
void timedDoorClose() {
  doorClose();
  timerState = false;
}

void doorTimer(){
  if(timerSwitch){
    timer.enable(timerId);
    timerState = true;
  }
}

void timerEnable() {
  if(timerSwitch){
    // do nothing
  } else {
    timerSwitch=true;
  }
}


void timerDisable() {
  if(timerSwitch){
    timerSwitch=false;
  } else {
    // do nothing
  }
}

void respond() { // server response in json with global details
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");
  server.sendContent("{\"DoorData\": [ { \"status\""+ String(isOpen) +"\"OpenCount\": \"" + openCountStr + "\", \"ClosedCount\": \"" + closedCountStr +"\", \"TimerState\": " + String(timerSwitch) +"\" } ] }");
  server.client().stop(); // Stop is needed because we sent no content length
}

void intitializeSite() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(webPage);
  server.client().stop(); // Stop is needed because we sent no content length
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
  server.on("/", intitializeSite);
  server.on("/DoorOpen", doorOpen);
  server.on("/DoorClose", doorClose);
  server.on("/DisableTimer", timerDisable);
  server.on("/EnableTimer", timerEnable);
  server.on("/CheckDoor", checkDoor);
  server.begin();
  timerState = false;
}

void loop()
{
  timer.run();
  isOpen = magCheck();
  if(doorStatus){
    if(timerState){
      timer.disable(timerId);
      timerState = false;
    } else{
      // timer is not running
    }
  } else {
    if(timerState){
      // timer is already running
    } else{
      timer.enable(timerId);
      timerState = true;
    }
  }
  server.handleClient();
}
