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
#define IDLE_MIN 30
// #define ENABLE_LOGGING // Comment/Uncomment to enable logging

#define NUM_LEDS 250
#define DATA_PIN 3
#define BUZZER_PIN 5

const byte DNS_PORT = 53;
DNSServer dnsServer;

uint32_t BoardUpdate = 1000;
uint32_t BuzzerDuration = 2000;

// the XML array size needs to be bigger that your maximum expected size.
char XML[512];

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
uint32_t LastInteraction = 0;
bool offMode = false;

void updateScoreboard()
{
  if (!offMode)
  {
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

    // updateIndicator(homePosession, nPos == 0, CRGB::Red);
    // updateIndicator(visitorPosession, nPos == 1, CRGB::Red);
    // updateIndicator(homeBonus, bBonusHome == -1, CRGB::Green);
    // updateIndicator(visitorBonus, bBonusVisitor, CRGB::Green);

    FastLED.show();
  }
}

void HomeScoreUp1() {
  nHomeScore++;
  registerInteraction("HomeScoreUp1");
}
void HomeScoreUp2() {
  nHomeScore += 2;
  registerInteraction("HomeScoreUp2");
}
void HomeScoreUp3() {
  nHomeScore += 3;
  registerInteraction("HomeScoreUp3");
}
void HomeScoreReset() {
  nHomeScore = 0;
  registerInteraction("HomeScoreReset");
}
void HomeScoreDown1() {
  nHomeScore--;
  registerInteraction("HomeScoreDown1");
}
void PeriodUp() {
  nPeriod++;
  if (nPeriod > 6)
  {
    nPeriod = 1;
  }
  registerInteraction("PeriodUp");
}
void VisitorScoreDown1() {
  nVisitorScore--;
  registerInteraction("VisitorScoreDown1");
}
void VisitorScoreReset() {
  nVisitorScore = 0;
  registerInteraction("VisitorScoreReset");
}
void VisitorScoreUp1() {
  nVisitorScore++;
  registerInteraction("VisitorScoreUp1");
}
void VisitorScoreUp2() {
  nVisitorScore += 2;
  registerInteraction("VisitorScoreUp2");
}
void VisitorScoreUp3() {
  nVisitorScore += 3;
  registerInteraction("VisitorScoreUp3");
}
void HomeBonus() {
  bBonusHome = !bBonusHome;
  registerInteraction("HomeBonus");
}
void HomePos() {
  nPos = 0;
  registerInteraction("HomePos");
}
void VisitorPos() {
  nPos = 1;
  registerInteraction("VisitorPos");
}
void VisitorBonus() {
  bBonusVisitor = !bBonusVisitor;
  registerInteraction("VisitorBonus");
}
void StartStopTimer() {
  bTimeRunning = !bTimeRunning;
  registerInteraction("StartStopTimer");
}
void TimeDn1() {
  nSeconds--;
  registerInteraction("TimeDn1");
}
void TimeDn10() {
  nSeconds-=10;
  registerInteraction("TimeDn10");
}
void TimeDn60() {
  nSeconds-=60;
  registerInteraction("TimeDn60");
}
void TimeUp60() {
  nSeconds+=60;
  registerInteraction("TimeUp60");
}
void TimeUp10() {
  nSeconds+=10;
  registerInteraction("TimeUp10");
}
void TimeUp1() {
  nSeconds++;
  registerInteraction("TimeUp1");
}
void TimerSet12() {
  bTimeRunning = false;
  nSeconds = 720;
  registerInteraction("TimerSet12");
}
void TimerSet10() {
  bTimeRunning = false;
  nSeconds = 600;
  registerInteraction("TimerSet10");
}
void TimerSet0() {
  bTimeRunning = false;
  nSeconds = 0;
  registerInteraction("TimerSet0");
}
void TimerSet2() {
  bTimeRunning = false;
  nSeconds = 120;
  registerInteraction("TimerSet2");
}
void TimerSet20() {
  bTimeRunning = false;
  nSeconds = 1200;
  registerInteraction("TimerSet20");
}
void Buzzer() {
  if (!bBuzzing)
  {
    bBuzzing = true;
    BuzzerStart = millis();
  }
}

void handleNotFound() {
  server.send(200, "text/plain", "");
}
void registerInteraction(const char interactionName[]) {
  LastInteraction = millis();
  if (offMode)
  {
#ifdef ENABLE_LOGGING
    Serial.println("On Mode");
#endif
    offMode = false;
  }
  server.send(200, "text/plain", "");
  #ifdef ENABLE_LOGGING
  Serial.print("Interaction Registered: ");
  Serial.println(interactionName);
  #endif
}
void setup() {  
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, BGR>(leds, NUM_LEDS);

  // LED Boot Test:
  updateDigit(homeScore, 188, CRGB::Red);
  updateDigit(visitorScore, 188, CRGB::Red);
  updateDigit(timeMinutes, 88, CRGB::Yellow);
  updateDigit(timeSeconds, 88, CRGB::Yellow);
  updateDigit(period, 8, CRGB::Green);
//   updateIndicator(homePosession, true, CRGB::Red);
//   updateIndicator(visitorPosession, true, CRGB::Red);
//   updateIndicator(homeBonus, true, CRGB::Green);
//   updateIndicator(visitorBonus, true, CRGB::Green);
  FastLED.show();
  delay(8000);
  
  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();
  // maybe disable watch dog timer 1 if needed
  // disableCore1WDT();

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
        registerInteraction("Timer");
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
  if ((millis() - LastInteraction > IDLE_MIN * 60000) && !offMode)
  {
    offMode = true;
#ifdef ENABLE_LOGGING
    Serial.println("Off Mode");
#endif
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = offColor;
    }
    FastLED.show();
    nPeriod = 1;
    nSeconds = 0; 
    nHomeScore = 0;
    nVisitorScore = 0;
    bBonusHome = false;
    bBonusVisitor = false;
    nPos = 0; // 0 home; 1 visitors
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
  sprintf(buf, "<Score>%02d - %02d</Score>\n", nHomeScore, nVisitorScore);
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
  Serial.print("Connected: ");
  Serial.println(WiFi.SSID());
  Serial.print("["); Serial.print(AP_SSID); Serial.println("]");
}