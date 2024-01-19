// Control stuff borrowed from https://github.com/KrisKasprzak/ESP32_WebPage
// https://www.youtube.com/watch?v=pL3dhGtmcMY

#include <WiFi.h>         
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WebServer.h>    
#include <FastLED.h>
#include "ControlPage.h"  // .h file that stores your html page code

#define AP_SSID "Keverian Scoreboard"
#define AP_PASS "PASSSWORD"

#define NUM_LEDS 250
#define DATA_PIN 3
#define BUZZER_PIN 5

const byte DNS_PORT = 53;
DNSServer dnsServer;

uint32_t BoardUpdate = 1000;
uint32_t BuzzerDuration = 2000;

// the XML array size needs to be bigger that your maximum expected size.
char XML[128];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress apIP(172, 217, 28, 1);
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
DigitElement timeMinutes = {50, false, 2};
DigitElement timeSeconds = {93, false, 2};
DigitElement visitorScore = {136, true, 2};
DigitElement period = {186, false, 1};

IndicatorElement homeBonus = {209, 4};
IndicatorElement visitorBonus = {174, 4};
IndicatorElement homePosession = {203, 6};
IndicatorElement visitorPosession = {177, 6};

CRGB leds[NUM_LEDS]; 
CRGB onColor = CRGB::White;
CRGB offColor = CRGB::Black;

void stringToDigit(const char* str, int ledStartIndex, CRGB colorOverride)
{
  leds[ledStartIndex]    = str[0]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+1]  = str[4]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+2]  = str[8]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+3]  = str[12]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+4]  = str[16]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+5]  = str[20]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+6]  = str[24]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+7]  = str[25]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+8]  = str[26]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+9]  = str[27]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+10] = str[23]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+11] = str[19]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+12] = str[15]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+13] = str[11]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+14] = str[7]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+15] = str[3]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+16] = str[2]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+17] = str[1]   == '#' ? colorOverride : offColor;
  leds[ledStartIndex+19] = str[13]  == '#' ? colorOverride : offColor;
  leds[ledStartIndex+20] = str[14]  == '#' ? colorOverride : offColor;
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
    currentRoot += 22;
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
int nSeconds = 0; 
int nHomeScore = 0;
int nVisitorScore = 0;
bool bBonusHome = false;
bool bBonusVisitor = false;
int nPos = 0; // 0 home; 1 visitors
bool bTimeRunning = false;
bool bBuzzing = false;
uint32_t BuzzerStart = 0;

void updateScoreboard()
{
//  Serial.println("Update");
  nHomeScore = constrain(nHomeScore, 0, 199);
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  nPeriod = constrain(nPeriod, 0, 9);
  nSeconds = constrain(nSeconds, 0, 3600);
  int minutesCalc = nSeconds / 60;
  int secondsCalc = nSeconds % 60;

  updateDigit(homeScore, nHomeScore, CRGB::Red);
  updateDigit(visitorScore, nVisitorScore, CRGB::Red);
  updateDigit(timeMinutes, minutesCalc, CRGB::Yellow);
  updateDigit(timeSeconds, secondsCalc, CRGB::Yellow);
  updateDigit(period, nPeriod, CRGB::Green);
//  updateIndicator(homePosession, nPos == 0, CRGB::Red);
//  updateIndicator(visitorPosession, nPos == 1, CRGB::Red);
//  updateIndicator(homeBonus, bBonusHome == -1, CRGB::Green);
//  updateIndicator(visitorBonus, bBonusVisitor, CRGB::Green);

  FastLED.show();
  server.send(200, "text/plain", "");
}

void HomeScoreUp1() {
Serial.println("HomeScoreUp1");
  nHomeScore++;
  server.send(200, "text/plain", "");
}
void HomeScoreUp2() {
Serial.println("HomeScoreUp2");
  nHomeScore += 2;
  server.send(200, "text/plain", "");
}
void HomeScoreUp3() {
Serial.println("HomeScoreUp3");
  nHomeScore += 3;
  server.send(200, "text/plain", "");
}
void HomeScoreReset() {
  Serial.println("HomeScoreReset");
  nHomeScore = 0;
  server.send(200, "text/plain", "");
}
void HomeScoreDown1() {
Serial.println("HomeScoreDown1");
  nHomeScore--;
  server.send(200, "text/plain", "");
}
void PeriodUp() {
Serial.println("PeriodUp");
  nPeriod++;
  if (nPeriod > 6)
  {
    nPeriod = 1;
  }
  server.send(200, "text/plain", "");
}
void VisitorScoreDown1() {
Serial.println("VisitorScoreDown1");
  nVisitorScore--;
  server.send(200, "text/plain", "");
}
void VisitorScoreReset() {
  Serial.println("VisitorScoreReset");
  nVisitorScore = 0;
  server.send(200, "text/plain", "");
}
void VisitorScoreUp1() {
Serial.println("VisitorScoreUp1");
  nVisitorScore++;
  server.send(200, "text/plain", "");
}
void VisitorScoreUp2() {
Serial.println("VisitorScoreUp2");
  nVisitorScore += 2;
  server.send(200, "text/plain", "");
}
void VisitorScoreUp3() {
Serial.println("VisitorScoreUp3");
  nVisitorScore += 3;
  server.send(200, "text/plain", "");
}
void HomeBonus() {
  Serial.println("HomeBonus");
  bBonusHome = !bBonusHome;
  server.send(200, "text/plain", "");
}
void HomePos() {
  Serial.println("HomePos");
  nPos = 0;
  server.send(200, "text/plain", "");
}
void VisitorPos() {
  Serial.println("VisitorPos");
  nPos = 1;
  server.send(200, "text/plain", "");
}
void VisitorBonus() {
  Serial.println("VisitorBonus");
  bBonusVisitor = !bBonusVisitor;
  server.send(200, "text/plain", "");
}
void StartStopTimer() {
  Serial.println("StartStopTimer");
  bTimeRunning = !bTimeRunning;
  server.send(200, "text/plain", "");
}
void TimeDn1() {
Serial.println("TimeDn1");
  nSeconds--;
  server.send(200, "text/plain", "");
}
void TimeDn10() {
  Serial.println("TimeDn10");
  nSeconds-=10;
  server.send(200, "text/plain", "");
}
void TimeDn60() {
  Serial.println("TimeDn60");
  nSeconds-=60;
  server.send(200, "text/plain", "");
}
void TimeUp60() {
  Serial.println("TimeUp60");
  nSeconds+=60;
  server.send(200, "text/plain", "");
}
void TimeUp10() {
  Serial.println("TimeUp10");
  nSeconds+=10;
  server.send(200, "text/plain", "");
}
void TimeUp1() {
Serial.println("TimeUp1");
  nSeconds++;
  server.send(200, "text/plain", "");
}
void TimerSet12() {
  Serial.println("TimerSet12");
  nSeconds = 720;
  server.send(200, "text/plain", "");
}
void TimerSet10() {
  Serial.println("TimerSet10");
  nSeconds = 600;
  server.send(200, "text/plain", "");
}
void TimerSet0() {
  Serial.println("TimerSet0");
  nSeconds = 0;
  server.send(200, "text/plain", "");
}
void TimerSet2() {
  Serial.println("TimerSet2");
  nSeconds = 120;
  server.send(200, "text/plain", "");
}
void TimerSet20() {
  Serial.println("TimerSet20");
  nSeconds = 1200;
  server.send(200, "text/plain", "");
}
void Buzzer() {
  if (!bBuzzing)
  {
    bBuzzing = true;
    BuzzerStart = millis();
    server.send(200, "text/plain", "");
  }
}

void handleNotFound() {
  server.send(200, "text/plain", "");
}

void setup() {
  // standard stuff here
  
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(leds, NUM_LEDS);

  // LED Boot Test:
  updateDigit(homeScore, 188, CRGB::Red);
  updateDigit(visitorScore, 188, CRGB::Red);
  updateDigit(timeMinutes, 88, CRGB::Yellow);
  updateDigit(timeSeconds, 88, CRGB::Yellow);
  updateDigit(period, 8, CRGB::Green);
//  updateIndicator(homePosession, true, CRGB::Red);
//  updateIndicator(visitorPosession, true, CRGB::Red);
//  updateIndicator(homeBonus, true, CRGB::Green);
//  updateIndicator(visitorBonus, true, CRGB::Green);
  delay(3000);
  
  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  //  disableCore1WDT();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  delay(100);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  dnsServer.start(DNS_PORT, "*", apIP);
  // Set up mDNS responder:
  //   the fully-qualified domain name is "scoreboard.local"
  if (!MDNS.begin("scoreboard")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  
  printWifiStatus();

  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  server.on("/HomeScoreUp1", HomeScoreUp1);
  server.on("/HomeScoreUp2", HomeScoreUp2);
  server.on("/HomeScoreUp3", HomeScoreUp3);
  server.on("/HomeScoreReset", HomeScoreReset);
  server.on("/HomeScoreDown1", HomeScoreDown1);
  server.on("/PeriodUp", PeriodUp);
  server.on("/VisitorScoreDown1", VisitorScoreDown1);
  server.on("/VisitorScoreReset", VisitorScoreReset);
  server.on("/VisitorScoreUp1", VisitorScoreUp1);
  server.on("/VisitorScoreUp2", VisitorScoreUp2);
  server.on("/VisitorScoreUp3", VisitorScoreUp3);
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

  // replay to all requests with same HTML
  server.onNotFound([]() {
      server.send(200, "text/html", PAGE_MAIN);
  });
  // finally begin the server
  server.begin();
}

void loop() {
  if ((millis() - BoardUpdate) >= 1000) {
    BoardUpdate = millis();
    if (bTimeRunning)
      {
        nSeconds--;
        if (nSeconds == 0)
        {
          bTimeRunning = false;
          Buzzer();
        }
      }
      updateScoreboard();
  }
  if (bBuzzing)
  {
    if (millis() - BuzzerStart < BuzzerDuration)
    {
      digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
      digitalWrite(BUZZER_PIN, LOW);
      bBuzzing = false;
    }
  }
  // call handleClient repeatedly--otherwise the web page
  // will not get instructions
  server.handleClient();
  dnsServer.processNextRequest();
}

void SendWebsite() 
{
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}

// code to send the main web page
void SendXML() {
  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");
  sprintf(buf, "<Time>%02d:%02d</Time>\n", nSeconds / 60, nSeconds % 60);
  strcat(XML, buf);
  sprintf(buf, "<HomeScore>%d</HomeScore>\n", nHomeScore);
  strcat(XML, buf);
  sprintf(buf, "<VisitorScore>%d</VisitorScore>\n", nVisitorScore);
  strcat(XML, buf);
  sprintf(buf, "<Period>%d</Period>\n", nPeriod);
  strcat(XML, buf);
  sprintf(buf, "<Pos>%d</Pos>\n", nPos);
  strcat(XML, buf);
  sprintf(buf, "<HomeBonus>%d</HomeBonus>\n", bBonusHome ? 1 : 0);
  strcat(XML, buf);
  sprintf(buf, "<VisitorBonus>%d</VisitorBonus>\n", bBonusVisitor ? 1 : 0);
  strcat(XML, buf);
  sprintf(buf, "<TimerRunning>%d</TimerRunning>\n", bTimeRunning ? 1 : 0);
  strcat(XML, buf);
  strcat(XML, "</Data>\n");
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
  Serial.print("["); Serial.print(AP_SSID); Serial.println("]");
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
