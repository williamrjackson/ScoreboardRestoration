/*


  this example will show
  1. how to use and ESP 32 for reading pins
  2. building a web page for a client (web browser, smartphone, smartTV) to connect to
  3. sending data from the ESP to the client to update JUST changed data
  4. sending data from the web page (like a slider or button press) to the ESP to tell the ESP to do something

  If you are not familiar with HTML, CSS page styling, and javascript, be patient, these code platforms are
  not intuitive and syntax is very inconsitent between platforms

  I know of 4 ways to update a web page
  1. send the whole page--very slow updates, causes ugly page redraws and is what you see in most examples
  2. send XML data to the web page that will update just the changed data--fast updates but older method
  3. JSON strings which are similar to XML but newer method
  4. web sockets very very fast updates, but not sure all the library support is available for ESP's

  I use XML here...

  compile options
  1. esp32 dev module
  2. upload speed 921600
  3. cpu speed 240 mhz
  flash speed 80 mhz
  flash mode qio
  flash size 4mb
  partition scheme default


  NOTE if your ESP fails to program press the BOOT button during programm when the IDE is "looking for the ESP"

  The MIT License (MIT)

  code writen by Kris Kasprzak
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  On a personal note, if you develop an application or product using this code 
  and make millions of dollars, I'm happy for you!

*/

#include <WiFi.h>         // standard library
#include <WebServer.h>    // standard library
#include "ControlPage.h"  // .h file that stores your html page code
#include <FastLED.h>

// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
#define USE_INTRANET

// replace this with your homes intranet connect parameters
#define LOCAL_SSID "SSID"
#define LOCAL_PASS "PASSWORD"

// once  you are read to go live these settings are what you client will connect to
#define AP_SSID "KeverianScoreboard"
#define AP_PASS "Karate*"

#define NUM_LEDS 212
#define DATA_PIN 6

uint32_t BoardUpdate = 0;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[2048];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

const char* zero =   "####"
                     "#..#"
                     "#..#"
                     "#..#"
                     "#..#"
                     "#..#"
                     "####";

const char* one =    "...#" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#";

const char* two =    "####" 
                     "...#" 
                     "...#" 
                     "####" 
                     "#..." 
                     "#..." 
                     "####";

const char* three =  "####" 
                     "...#" 
                     "...#" 
                     "####" 
                     "...#" 
                     "...#" 
                     "####";

const char* four =   "#..#" 
                     "#..#" 
                     "#..#" 
                     "####" 
                     "...#" 
                     "...#" 
                     "...#";

const char* five =   "####" 
                     "#..." 
                     "#..." 
                     "####" 
                     "...#" 
                     "...#" 
                     "####";

const char* six =    "#..." 
                     "#..." 
                     "#..." 
                     "####" 
                     "#..#" 
                     "#..#" 
                     "####";

const char* seven =  "####" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#" 
                     "...#";

const char* eight =  "####" 
                     "#..#" 
                     "#..#" 
                     "####" 
                     "#..#" 
                     "#..#" 
                     "####";

const char* nine =   "####" 
                     "#..#" 
                     "#..#" 
                     "####" 
                     "...#" 
                     "...#" 
                     "####";
const char* numberConfigs[] {zero, one, two, three, four, five, six, seven, eight, nine};

struct DigitElement
{
   int baseOffset;
   bool hasOneHundred;
   int segmentedDigitCount;
};
struct IndicatorElement
{
  int baseOffset;
  int lightCount;
};

DigitElement homeScore = {0, true, 2};
DigitElement visitorScore = {127, true, 2};
DigitElement timeMinutes = {47, false, 2};
DigitElement timeSeconds = {87, false, 2};
DigitElement period = {183, false, 1};

IndicatorElement homeBonus = {209, 3};
IndicatorElement visitorBonus = {174, 3};
IndicatorElement homePosession = {203, 6};
IndicatorElement visitorPosession = {177, 6};

CRGB leds[NUM_LEDS]; 
CRGB onColor = CRGB::White;
CRGB offColor = CRGB::Black;

void stringToDigit(char* str, int ledStartIndex)
{
  stringToDigit(str, ledStartIndex, onColor);
}
void stringToDigit(char* str, int ledStartIndex, CRGB colorOverride)
{
  leds[ledStartIndex]    = str[0]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+1]  = str[1]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+2]  = str[2]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+3]  = str[3]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+4]  = str[7]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+5]  = str[4]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+6]  = str[8]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+7]  = str[11] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+8]  = str[15] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+9]  = str[14] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+10] = str[13] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+11] = str[12] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+12] = str[16] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+13] = str[19] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+14] = str[23] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+15] = str[20] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+16] = str[24] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+17] = str[25] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+18] = str[26] == '#' ? colorOverride : offColor;
  leds[ledStartIndex+19] = str[27] == '#' ? colorOverride : offColor;
}

//*****************************************************
void updateDigit(DigitElement el, int value)
{
  updateDigit(el, value, onColor);
}
void updateDigit(DigitElement el, int value, CRGB colorOverride)
{
  int currentRoot = el.baseOffset;
  if (el.hasOneHundred)
  {
    CRGB hundred = (value > 99) ? colorOverride : offColor;

    for (int i = currentRoot; i < currentRoot + 7; i++) 
    {
      leds[i] = hundred;
    }
    currentRoot += 7;
  }
  if (el.segmentedDigitCount > 1)
  {
    int tensIndex = (value / 10) % 10;
    stringToDigit(numberConfigs[tensIndex], currentRoot, colorOverride);
    currentRoot += 20;
  }
  int onesIndex = (value % 10);
  stringToDigit(numberConfigs[onesIndex], currentRoot, colorOverride);
}
void updateIndicator(IndicatorElement el, bool value)
{
  updateIndicator(el, value, onColor);
}
void updateIndicator(IndicatorElement el, bool value, CRGB colorOverride)
{
   for (int i = el.baseOffset; i < el.baseOffset + el.lightCount; i++) {
     leds[i] = value ? colorOverride : offColor;
   }
}

//********************************************************
int nPeriod = 1;
int seconds = 0; 
int nHomeScore = 0;
int nVisitorScore = 0;
bool bTimeRunning = false;
int bonus = 0; // -1 home; 0 none; 1 visitors
int posession = 0; // 0 home; 1 visitors

void updateScoreboard()
{
  nHomeScore = constrain(nHomeScore, 0, 199);
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  nPeriod = constrain(nPeriod, 0, 9);
  seconds = constrain(seconds, 0, 3600);
  int minutesCalc = seconds / 60;
  int secondsCalc = seconds % 60;
  updateDigit(homeScore, nHomeScore);
  updateDigit(visitorScore, nVisitorScore);
  updateDigit(period, nPeriod);
  updateDigit(timeMinutes, minutesCalc);
  updateDigit(timeSeconds, secondsCalc);

  updateIndicator(homePosession, posession == 0);
  updateIndicator(visitorPosession, posession == 1);
  updateIndicator(homeBonus, bonus == -1);
  updateIndicator(visitorBonus, bonus == 1);

  FastLED.show();
  server.send(200, "text/plain", ""); //Send web page
}

void HomeScoreUp1() {
  nHomeScore++;
  server.send(200, "text/plain", ""); //Send web page
}
void HomeScoreReset() {
  nHomeScore = 0;
  server.send(200, "text/plain", ""); //Send web page
}
void HomeScoreDown1() {
    nHomeScore--;
  server.send(200, "text/plain", ""); //Send web page
}
void PeriodUp() {
  nPeriod++;
  if (nPeriod > 4)
  {
    nPeriod = 1;
  }
  server.send(200, "text/plain", ""); //Send web page
}
void VisitorScoreDown1() {
  nVisitorScore--;
  server.send(200, "text/plain", ""); //Send web page
}
void VisitorScoreReset() {
  nVisitorScore = 0;
  server.send(200, "text/plain", ""); //Send web page
}
void VisitorScoreUp1() {
  nVisitorScore++;
  server.send(200, "text/plain", ""); //Send web page
}
void HomeBonus() {
  bonus = (bonus == -1) ? 0 : -1;
  server.send(200, "text/plain", ""); //Send web page
}
void HomePos() {
  posession = 0;
  server.send(200, "text/plain", ""); //Send web page
}
void VisitorPos() {
  posession = 1;
  server.send(200, "text/plain", ""); //Send web page
}
void VisitorBonus() {
  bonus = (bonus == 1) ? 0 : 1;
  server.send(200, "text/plain", ""); //Send web page
}
void StartStopTimer() {
  bTimeRunning = !bTimeRunning;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeDn1() {
  seconds--;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeDn10() {
  seconds-=10;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeDn60() {
  seconds-=60;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeUp60() {
  seconds+=60;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeUp10() {
  seconds+=10;
  server.send(200, "text/plain", ""); //Send web page
}
void TimeUp1() {
  seconds++;
  server.send(200, "text/plain", ""); //Send web page
}
void TimerSet12() {
  seconds = 720;
  server.send(200, "text/plain", ""); //Send web page
}
void TimerSet10() {
  seconds = 600;
  server.send(200, "text/plain", ""); //Send web page
}
void TimerSet0() {
  seconds = 0;
  server.send(200, "text/plain", ""); //Send web page
}
void TimerSet2() {
  seconds = 120;
  server.send(200, "text/plain", ""); //Send web page
}
void TimerSet20() {
  seconds = 1200;
  server.send(200, "text/plain", ""); //Send web page
}
void Buzzer() {
  // TODO:
  Serial.println("Zzzzzt!");
  server.send(200, "text/plain", ""); //Send web page
}


void setup() {

  // standard stuff here
  Serial.begin(9600);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  pinMode(DATA_PIN, OUTPUT);

  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  //  disableCore1WDT();

  // just an update to progress
  Serial.println("starting server");

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: "); Serial.println(Actual_IP);
#endif

  printWifiStatus();


  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  // upon ESP getting /UPDATE_SLIDER string, ESP will execute the UpdateSlider function
  // same notion for the following .on calls
  // add as many as you need to process incoming strings from your web page
  // as you can imagine you will need to code some javascript in your web page to send such strings
  // this process will be documented in the SuperMon.h web page code
  server.on("/HomeScoreUp1", HomeScoreUp1);
  server.on("/HomeScoreReset", HomeScoreReset);
  server.on("/HomeScoreDown1", HomeScoreDown1);
  server.on("/PeriodUp", PeriodUp);
  server.on("/VisitorScoreDown1", VisitorScoreDown1);
  server.on("/VisitorScoreReset", VisitorScoreReset);
  server.on("/VisitorScoreUp1", VisitorScoreUp1);
  server.on("/HomeBonus", HomeBonus);
  server.on("/HomePos", HomePos);
  server.on("/VisitorPos", VisitorPos);
  server.on("/VisitorBonus", VisitorBonus);
  server.on("/StartStopTimer", StartStopTimer);
  server.on("/TimeDn1", TimeDn1);
  server.on("/TimeDn10", TimeDn10);
  server.on("/TimeDn60", TimeDn60);
  server.on("/TimeUp60", TimeUp60);
  server.on("/TimeUp10", TimeUp10);
  server.on("/TimeUp1", TimeUp1);
  server.on("/TimerSet12", TimerSet12);
  server.on("/TimerSet10", TimerSet10);
  server.on("/TimerSet0", TimerSet0);
  server.on("/TimerSet2", TimerSet2);
  server.on("/TimerSet20", TimerSet20);
  server.on("/Buzzer", Buzzer);

  // finally begin the server
  server.begin();

}

void loop() {

  // you main loop that measures, processes, runs code, etc.
  // note that handling the "on" strings from the web page are NOT in the loop
  // that processing is in individual functions all managed by the wifi lib

  // in my example here every 50 ms, i measure some analog sensor data (my finger dragging over the pins
  // and process accordingly
  // analog input can be from temperature sensors, light sensors, digital pin sensors, etc.
  if ((millis() - BoardUpdate) >= 1000) {
    //Serial.println("Reading Sensors");
    BoardUpdate = millis();
    if (bTimeRunning)
      {
        seconds--;
        if (seconds == 0)
        {
          bTimeRunning = false;
          Buzzer();
        }
      }
      updateScoreboard();
  }

  // no matter what you must call this handleClient repeatidly--otherwise the web page
  // will not get instructions to do something
  server.handleClient();

}


// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {

  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML() {
  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  sprintf(buf, "<Time>%d:%d</Time>\n", seconds / 60, seconds % 60);
  strcat(XML, buf);
  sprintf(buf, "<HomeScore>%d</HomeScore>\n", nHomeScore);
  strcat(XML, buf);
  sprintf(buf, "<VisitorScore>%d</VisitorScore>\n", nVisitorScore);
  strcat(XML, buf);
  sprintf(buf, "<Period>%d</Period>\n", nPeriod);
  strcat(XML, buf);
  sprintf(buf, "<Pos>%d</Pos>\n", nPos);
  strcat(XML, buf);
  sprintf(buf, "<Bonus>%d</Bonus>\n", nBonus);
  strcat(XML, buf);

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  // Serial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);


}

// I think I got this code from the wifi example
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}

// end of code
