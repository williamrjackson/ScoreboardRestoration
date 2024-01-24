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

#define NUM_LEDS 254
#define DATA_PIN 3
#define BUZZER_PIN 5

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(172, 217, 28, 1);
IPAddress ip;

// gotta create a server
WebServer server(80);

// the XML array size needs to be bigger that your maximum expected size.
char XML[256];
// buffer for XML char operations
char buf[32];

uint32_t BoardUpdate = 1000;
uint32_t BuzzerDuration = 1000;
uint32_t millisCache = 0;

//  LED Layout:
//
//  6   0  17  16  15
//  5   1          14
//  4   2          13
//  3   3  19  20  12
//  2   4          11
//  1   5          10
//  0   6  7   8   9

// Digit LED Configs
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
// Access each map by corresponding index
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

IndicatorElement homeBonus = {236, 4};
IndicatorElement visitorBonus = {240, 4};
IndicatorElement homePosession = {244, 6};
IndicatorElement visitorPosession = {249, 6};

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
// Total seconds on clock
int nSeconds = 0; 
int nHomeScore = 0;
int nVisitorScore = 0;
bool bBonusHome = false;
bool bBonusVisitor = false;
// Possession: 0 home; 1 visitors; 2 both for all-on mode
int nPos = 0; 
bool bTimeRunning = false;
bool bBuzzing = false;
uint32_t BuzzerStart = 0;
uint32_t LastInteraction = 0;
bool offMode = false;
// To to hold fully lit - requires 6 consecutive "HomeScoreSet: 0" commands.
int nConsecutiveResets = 0; 

void updateScoreboard()
{
  if (!offMode)
  {
    int home = constrain(nHomeScore, 0, 199);
    int visitor = constrain(nVisitorScore, 0, 199);
    int periodWrite = constrain(nPeriod, 0, 9);
    int seconds = constrain(nSeconds, 0, 6039);
    int pos = constrain(nPos, 0, 2);
    int minutesCalc = nSeconds / 60;
    int secondsCalc = nSeconds % 60;
    bool bonusHomeState = bBonusHome;
    bool bonusVisState = bBonusVisitor;

    if (nConsecutiveResets == 6)
    {
#ifdef ENABLE_LOGGING
      Serial.println("[Secret Maintenance Mode]");
#endif
      home = 188;
      visitor = 188;
      minutesCalc = 88;
      secondsCalc = 88;
      periodWrite = 8;
      pos = 2;
      bonusHomeState = true;
      bonusVisState = true;

      LastInteraction = millis();
    }
    if (nConsecutiveResets > 6)
    {
#ifdef ENABLE_LOGGING
      Serial.println("[Secret Force Off]");
#endif
      LastInteraction = millis() - (IDLE_MIN * 60000);
    }
    updateDigit(homeScore, home, CRGB::Red);
    updateDigit(visitorScore, visitor, CRGB::Red);
    updateDigit(timeMinutes, minutesCalc, CRGB::Yellow);
    updateDigit(timeSeconds, secondsCalc, CRGB::Yellow);
    updateDigit(period, periodWrite, CRGB::Green);

    updateIndicator(homePosession, pos != 1, CRGB::Red);
    updateIndicator(visitorPosession, pos != 0, CRGB::Red);
    updateIndicator(homeBonus, bonusHomeState, CRGB::Green);
    updateIndicator(visitorBonus, bonusVisState, CRGB::Green);

    FastLED.show();
  }
}

void HomeScoreChange(int amt) {
  nHomeScore += amt;
  nHomeScore = constrain(nHomeScore, 0, 199);
  registerInteraction("HomeScoreChange", amt);
}

void HomeScoreSet(int val) {
  nHomeScore = val;
  nHomeScore = constrain(nHomeScore, 0, 199);
  registerInteraction("HomeScoreSet", val);
}

void PeriodChange() {
  nPeriod++;
  if (nPeriod > 6 || nPeriod < 1)
  {
    nPeriod = 1;
  }
  registerInteraction("PeriodChange");
}

void VisitorScoreChange(int amt) {
  nVisitorScore += amt;
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  registerInteraction("VisitorScoreChange", amt);
}

void VisitorScoreSet(int val) {
  nVisitorScore = val;
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  registerInteraction("VisitorScoreSet", val);
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
  if (bTimeRunning) {
    TimerStop();
  } else {
    TimerStart();
  }
}
void TimerStart() {
  bTimeRunning = true;
  registerInteraction("TimerState", 1);
}
void TimerStop() {
  bTimeRunning = false;
  registerInteraction("TimerState", 0);
}

void TimeChange(int seconds) {
  nSeconds += seconds;
  nSeconds = constrain(nSeconds, 0, 6039);
  registerInteraction("TimeChange", seconds);
}

void TimerSetMinutes(int minutes) {
  if (bTimeRunning) TimerStop();
  nSeconds = minutes * 60;
  nSeconds = constrain(nSeconds, 0, 6039);
  registerInteraction("TimerSet", nSeconds / 60);
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

void registerInteraction(const char interactionName[], int value)
{
  char interactionValue[64];
  sprintf(interactionValue, "%s: %d", interactionName, value);
  registerInteraction(interactionValue);
}
void registerInteraction(const char interactionName[]) {
  GenerateXML();
  LastInteraction = millis();
  // Count HomeScoreReset presses to enter 
  // Off(7) or Maintenance(6) Mode
  if (strcmp(interactionName, "HomeScoreSet: 0") == 0)
  {
    nConsecutiveResets++;
#ifdef ENABLE_LOGGING
    Serial.print("Consecutive Resets: ");
    Serial.println(nConsecutiveResets);
#endif
  }
  else
  {
    nConsecutiveResets = 0;
  }

  if (offMode)
  {
#ifdef ENABLE_LOGGING
    Serial.println("On Mode");
#endif
    offMode = false;
  }
  server.send(200, "text/plain", "");
#ifdef ENABLE_LOGGING
  Serial.print("Interaction Registered [");
  Serial.print(interactionName);
  Serial.println("]");
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
  GenerateXML();
  delay(8000);

  // if the page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  // disableCore0WDT();
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
  server.on("/HomeScoreUp1", [](){ HomeScoreChange(1); });
  server.on("/HomeScoreUp2", [](){ HomeScoreChange(2); });
  server.on("/HomeScoreUp3", [](){ HomeScoreChange(3); });
  server.on("/HomeScoreDown1", [](){ HomeScoreChange(-1); });
  server.on("/HomeScoreReset", [](){ HomeScoreSet(0); });
  server.on("/PeriodUp", PeriodChange);
  server.on("/VisitorScoreUp1", [](){ VisitorScoreChange(1); });
  server.on("/VisitorScoreUp2", [](){ VisitorScoreChange(2); });
  server.on("/VisitorScoreUp3", [](){ VisitorScoreChange(3); });
  server.on("/VisitorScoreDown1", [](){ VisitorScoreChange(-1); });
  server.on("/VisitorScoreReset", [](){ VisitorScoreSet(0); });
  server.on("/HomeBonus", HomeBonus);
  server.on("/HomePos", HomePos);
  server.on("/VisitorPos", VisitorPos);
  server.on("/VisitorBonus", VisitorBonus);
  server.on("/StartStopTimer", StartStopTimer);
  server.on("/TimeDn1", [](){ TimeChange(-1); });
  server.on("/TimeDn10", [](){ TimeChange(-10); });
  server.on("/TimeDn60", [](){ TimeChange(-60); });
  server.on("/TimeUp60", [](){ TimeChange(60); });
  server.on("/TimeUp10", [](){ TimeChange(10); });
  server.on("/TimeUp1", [](){ TimeChange(1); });
  server.on("/TimerSet0", [](){ TimerSetMinutes(0); });
  server.on("/TimerSet2", [](){ TimerSetMinutes(2); });
  server.on("/TimerSet10", [](){ TimerSetMinutes(10); });
  server.on("/TimerSet12", [](){ TimerSetMinutes(12); });
  server.on("/TimerSet20", [](){ TimerSetMinutes(20); });
  server.on("/Buzzer", Buzzer);

  // replay to all requests with same HTML
  server.onNotFound(handleNotFound);
  // finally begin the server
  server.begin();
}

void loop() {
  // Eventually millis() loops back to 0
  // Avoid getting stuck by resetting everything when it happens
  if (millis() < millisCache)
  {
#ifdef ENABLE_LOGGING
  Serial.println("Millis Loop");
#endif
    BoardUpdate = 0;
    LastInteraction = 0;
    BuzzerStart = 0;
  }
  millisCache = millis();
  if ((millis() - BoardUpdate) >= 1000) {
    BoardUpdate = millis();
    if (bTimeRunning)
    {
      TimeChange(-1);
      if (nSeconds == 0)
      {
        TimerStop();
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
    Serial.print("Off Mode: ");
    Serial.print(millis());
    Serial.print(" - ");
    Serial.print(LastInteraction);
    Serial.print("(");
    Serial.print(millis() - LastInteraction);
    Serial.println(")");
#endif
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = offColor;
    }
    FastLED.show();
    nConsecutiveResets = 0;
    nPeriod = 1;
    nSeconds = 0; 
    nHomeScore = 0;
    nVisitorScore = 0;
    bBonusHome = false;
    bBonusVisitor = false;
    nPos = 0; // 0 home; 1 visitors
    GenerateXML();
  }
  // call handleClient repeatedly--otherwise the web page
  // will not get instructions
  server.handleClient();
  dnsServer.processNextRequest();
}

void SendWebsite() {
  server.send(200, "text/html", PAGE_MAIN);
}

// code to send the main web page
void SendXML() {
    server.send(200, "text/xml", XML);
}

void GenerateXML() {
#ifdef ENABLE_LOGGING
  Serial.println("Generating xml...");
#endif
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
#ifdef ENABLE_LOGGING
  Serial.println(XML);
#endif
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("Connected: ");
  Serial.println(WiFi.SSID());
  Serial.print("["); Serial.print(AP_SSID); Serial.println("]");
}