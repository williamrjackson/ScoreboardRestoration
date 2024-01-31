// Control stuff borrowed from https://github.com/KrisKasprzak/ESP32_WebPage
// https://www.youtube.com/watch?v=pL3dhGtmcMY

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <FastLED.h>
#include "ControlPage.h"  // .h file that stores your html page code

#define ENABLE_LOGGING // Comment/Uncomment to enable logging

#define AP_SSID "Keverian Scoreboard"
#define AP_PASS "PASSSWORD"

// How many minutes of no interaction (or timer events) before automatically sleeping
#define IDLE_MIN 30

#define DATA_PIN 3
#define BUZZER_PIN 13

// LEDs available in full strip
#define NUM_LEDS 254
CRGB leds[NUM_LEDS];

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(172, 217, 28, 1);
IPAddress ip;

// Create a Server
WebServer server(80);

// XML array size needs to be bigger that your maximum expected size.
char XML[256];
// buffer for XML char operations
char buf[32];

// Scheduling vars
uint32_t boardUpdateTime = 1000;
uint32_t buzzerStartTime = 0;
uint32_t buzzerDuration = 1000;
uint32_t millisCache = 0;
uint32_t lastInteraction = 0;

// States
bool bTimeRunning = false;
bool bBuzzing = false;
bool sleepMode = false;

// To to hold fully lit - requires 6 consecutive "HomeScoreSet: 0" commands.
int nConsecutiveResets = 0;

// Global vars
//********************************************************
int nPeriod = 1;
// Total seconds on clock
int nSeconds = 0;
int nHomeScore = 0;
int nVisitorScore = 0;
bool bBonusHome = false;
bool bBonusVisitor = false;
// Possession: 0 home; 1 visitors; 2 both for all-on mode
int nPossession = 0;

//  LED Layout:
//
//     6   0  17  16  15
//     5   1          14
//     4   2          13
//     3   3  19  20  12
//     2   4          11
//     1   5          10
//     0   6  7   8   9

// Digit LED Maps
const char* zero =  "####"
                    "#..#"
                    "#..#"
                    "#..#"
                    "#..#"
                    "#..#"
                    "####";

const char* one =   "...#"
                    "...#"
                    "...#"
                    "...#"
                    "...#"
                    "...#"
                    "...#";

const char* two =   "####"
                    "...#"
                    "...#"
                    "####"
                    "#..."
                    "#..."
                    "####";

const char* three = "####"
                    "...#"
                    "...#"
                    "####"
                    "...#"
                    "...#"
                    "####";

const char* four =  "#..#"
                    "#..#"
                    "#..#"
                    "####"
                    "...#"
                    "...#"
                    "...#";

const char* five =  "####"
                    "#..."
                    "#..."
                    "####"
                    "...#"
                    "...#"
                    "####";

const char* six =   "#..."
                    "#..."
                    "#..."
                    "####"
                    "#..#"
                    "#..#"
                    "####";

// const char* six =   "####"
//                     "#..."
//                     "#..."
//                     "####"
//                     "#..#"
//                     "#..#"
//                     "####";

const char* seven = "####"
                    "...#"
                    "...#"
                    "...#"
                    "...#"
                    "...#"
                    "...#";

const char* eight = "####"
                    "#..#"
                    "#..#"
                    "####"
                    "#..#"
                    "#..#"
                    "####";

const char* nine =  "####"
                    "#..#"
                    "#..#"
                    "####"
                    "...#"
                    "...#"
                    "####";
// Access each map by its corresponding index
const char* numberLedMaps[]{ zero, one, two, three, four, five, six, seven, eight, nine };

// Scoreboard LED "cluster" objects
struct DigitElement {
  int baseOffset; // First LED index
  bool hasOneHundred; // Are the first 7 LEDS the "1" available on 3-digit panels?
  int segmentedDigitCount; // How many non-1 digits are there?
  CRGB defaultColor;
};
// Indicators are groups of consecutive LEDs that are treated
// as a single unit.
struct IndicatorElement {
  int baseOffset;
  int lightCount;
  CRGB defaultColor;
};

DigitElement homeScore = { 0, true, 2, CRGB::Red };
DigitElement timeMinutes = { 50, false, 2, CRGB::Yellow };
DigitElement timeSeconds = { 93, false, 2, CRGB::Yellow };
DigitElement visitorScore = { 136, true, 2, CRGB::Red };
DigitElement period = { 186, false, 1, CRGB::Green };

IndicatorElement homeBonus = { 236, 4, CRGB::Green };
IndicatorElement visitorBonus = { 240, 4, CRGB::Green };
IndicatorElement homePosession = { 244, 6, CRGB::Red };
IndicatorElement visitorPosession = { 249, 6, CRGB::Red };


void DigitFromMap(const char* str, int ledStartIndex, CRGB col) {
  leds[ledStartIndex] =       str[0] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 1] =   str[4] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 2] =   str[8] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 3] =   str[12] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 4] =   str[16] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 5] =   str[20] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 6] =   str[24] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 7] =   str[25] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 8] =   str[26] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 9] =   str[27] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 10] =  str[23] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 11] =  str[19] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 12] =  str[15] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 13] =  str[11] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 14] =  str[7] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 15] =  str[3] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 16] =  str[2] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 17] =  str[1] ==   '#' ? col : CRGB::Black;
  leds[ledStartIndex + 19] =  str[13] ==  '#' ? col : CRGB::Black;
  leds[ledStartIndex + 20] =  str[14] ==  '#' ? col : CRGB::Black;
}

//*****************************************************
void UpdateDigit(DigitElement el, int value) {
  UpdateDigit(el, value, el.defaultColor);
}
void UpdateDigit(DigitElement el, int value, CRGB col) {
  int currentRoot = el.baseOffset;
  if (el.hasOneHundred) {
    CRGB hundred = (value > 99) ? col : CRGB::Black;

    for (int i = currentRoot; i < currentRoot + 7; i++) {
      leds[i] = hundred;
    }
    currentRoot += 7;
  }
  if (el.segmentedDigitCount > 1) {
    int tensIndex = (value / 10) % 10;
    DigitFromMap(numberLedMaps[tensIndex], currentRoot, col);
    currentRoot += 22;
  }
  int onesIndex = (value % 10);
  DigitFromMap(numberLedMaps[onesIndex], currentRoot, col);
}

void UpdateIndicator(IndicatorElement el, bool value) {
  UpdateIndicator(el, value, el.defaultColor);
}
void UpdateIndicator(IndicatorElement el, bool value, CRGB col) {
  for (int i = el.baseOffset; i < el.baseOffset + el.lightCount; i++) {
    leds[i] = value ? col : CRGB::Black;
  }
}

// Update routines
//*****************************************************

void ScoreboardLedRoutine() {
  if ((millis() - boardUpdateTime) < 1000) { return; }
  boardUpdateTime = millis();
  if (sleepMode) { return; }

  if (bTimeRunning) {
    TimeChange(-1);
    if (nSeconds == 0) {
      TimerStop();
      BuzzerStart();
    }
  }

  int home = constrain(nHomeScore, 0, 199);
  int visitor = constrain(nVisitorScore, 0, 199);
  int periodWrite = constrain(nPeriod, 0, 9);
  int seconds = constrain(nSeconds, 0, 6039);
  int pos = constrain(nPossession, 0, 2);
  int minutesCalc = nSeconds / 60;
  int secondsCalc = nSeconds % 60;
  bool bonusHomeState = bBonusHome;
  bool bonusVisState = bBonusVisitor;

  if (nConsecutiveResets == 6) {
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

    lastInteraction = millis();
  }
  if (nConsecutiveResets > 6) {
#ifdef ENABLE_LOGGING
    Serial.println("[Secret Force to Sleep]");
#endif
    lastInteraction = millis() - (IDLE_MIN * 60000);
  }
  UpdateDigit(homeScore, home);
  UpdateDigit(visitorScore, visitor);
  UpdateDigit(timeMinutes, minutesCalc);
  UpdateDigit(timeSeconds, secondsCalc);
  UpdateDigit(period, periodWrite);

  UpdateIndicator(homePosession, pos != 1);
  UpdateIndicator(visitorPosession, pos != 0);
  UpdateIndicator(homeBonus, bonusHomeState);
  UpdateIndicator(visitorBonus, bonusVisState);

  FastLED.show();
}

void BuzzRoutine() {
  if (!bBuzzing) { return; }
  if (millis() - buzzerStartTime > buzzerDuration) {
    digitalWrite(BUZZER_PIN, LOW);
    bBuzzing = false;
  }
}
void IdleRoutine() {
  if (sleepMode) { return; }
  if (millis() - lastInteraction < IDLE_MIN * 60000) { return; }

  sleepMode = true;
#ifdef ENABLE_LOGGING
  Serial.println("Sleep Mode");
#endif
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  // Reset all values on sleep:
  nConsecutiveResets = 0;
  nPeriod = 1;
  nSeconds = 0;
  nHomeScore = 0;
  nVisitorScore = 0;
  bBonusHome = false;
  bBonusVisitor = false;
  nPossession = 0;  // 0 home; 1 visitors; 2 both
  GenerateXML();
}

// Interaction Functions
//*****************************************************

// Score Functions
void HomeScoreChange(int amt) {
  nHomeScore += amt;
  nHomeScore = constrain(nHomeScore, 0, 199);
  RegisterInteraction("HomeScoreChange", amt);
}
void VisitorScoreChange(int amt) {
  nVisitorScore += amt;
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  RegisterInteraction("VisitorScoreChange", amt);
}
void HomeScoreSet(int val) {
  nHomeScore = val;
  nHomeScore = constrain(nHomeScore, 0, 199);
  RegisterInteraction("HomeScoreSet", val);
}
void VisitorScoreSet(int val) {
  nVisitorScore = val;
  nVisitorScore = constrain(nVisitorScore, 0, 199);
  RegisterInteraction("VisitorScoreSet", val);
}

// Possession radio option
void Possession(int pos) {
  nPossession = pos;
  RegisterInteraction("Possession", nPossession);
}

// Bonus toggle functions
void HomeBonus() {
  bBonusHome = !bBonusHome;
  RegisterInteraction("HomeBonus");
}
void VisitorBonus() {
  bBonusVisitor = !bBonusVisitor;
  RegisterInteraction("VisitorBonus");
}

// Increment period. Loops 1-6
void PeriodChange() {
  nPeriod++;
  if (nPeriod > 6 || nPeriod < 1) {
    nPeriod = 1;
  }
  RegisterInteraction("PeriodChange");
}

// Timer Functions
void StartStopTimer() {
  if (bTimeRunning) {
    TimerStop();
  } else {
    TimerStart();
  }
}
void TimerStart() {
  bTimeRunning = true;
  RegisterInteraction("TimerState", 1);
}
void TimerStop() {
  bTimeRunning = false;
  RegisterInteraction("TimerState", 0);
}
void TimeChange(int seconds) {
  nSeconds += seconds;
  nSeconds = constrain(nSeconds, 0, 6039);
  RegisterInteraction("TimeChange", seconds);
}
void TimeSetMinutes(int minutes) {
  if (bTimeRunning) TimerStop();
  nSeconds = minutes * 60;
  nSeconds = constrain(nSeconds, 0, 6039);
  RegisterInteraction("TimerSet", nSeconds / 60);
}

// Buzzer function
void BuzzerStart() {
  if (bBuzzing) { return; }
  digitalWrite(BUZZER_PIN, HIGH);
  bBuzzing = true;
  buzzerStartTime = millis();
}

// Propegate/process button taps from the controller and timer events
void RegisterInteraction(const char interactionName[], int value) {
  char interactionValue[64];
  sprintf(interactionValue, "%s: %d", interactionName, value);
  RegisterInteraction(interactionValue);
}
void RegisterInteraction(const char interactionName[]) {
  GenerateXML();
  lastInteraction = millis();
  // Count HomeScoreReset presses to enter
  // Sleep(7) or Maintenance(6) Mode
  if (strcmp(interactionName, "HomeScoreSet: 0") == 0) {
    nConsecutiveResets++;
#ifdef ENABLE_LOGGING
    Serial.print("Consecutive Resets: ");
    Serial.println(nConsecutiveResets);
#endif
  } else {
    nConsecutiveResets = 0;
  }

  if (sleepMode) {
#ifdef ENABLE_LOGGING
    Serial.println("Wake up from sleep mode");
#endif
    sleepMode = false;
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
  UpdateDigit(homeScore, 188);
  UpdateDigit(visitorScore, 188);
  UpdateDigit(timeMinutes, 88);
  UpdateDigit(timeSeconds, 88);
  UpdateDigit(period, 8);
  UpdateIndicator(homePosession, true);
  UpdateIndicator(visitorPosession, true);
  UpdateIndicator(homeBonus, true);
  UpdateIndicator(visitorBonus, true);

  FastLED.show();
  GenerateXML();
  delay(8000);

  // if the page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // Maybe disable watch dog timers
  // disableCore0WDT();
  // disableCore1WDT();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  delay(100);
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  dnsServer.start(DNS_PORT, "*", apIP);
  // Set up mDNS responder:
  if (!MDNS.begin("scoreboard")) {
    // the fully-qualified domain name is "scoreboard.local"
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  printWifiStatus();

  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/HomeScore", []() {
    if (server.args()) {
      int val = server.arg(0).toInt();
      if (server.argName(0) == "change") HomeScoreChange(val);
      else if (server.argName(0) == "set") HomeScoreSet(val);
    }
  });
  server.on("/VisitorScore", []() {
    if (server.args()) {
      int val = server.arg(0).toInt();
      if (server.argName(0) == "change") VisitorScoreChange(val);
      else if (server.argName(0) == "set") VisitorScoreSet(val);
    }
  });
  server.on("/Time", []() {
    if (server.args()) {
      int val = server.arg(0).toInt();
      if (server.argName(0) == "change") TimeChange(val);
      else if (server.argName(0) == "set") TimeSetMinutes(val);
    }
  });
  server.on("/Possession", []() {
    if (server.args()) {
      int val = server.arg(0).toInt();
      Possession(val);
    }
  });
  server.on("/PeriodChange", PeriodChange);
  server.on("/HomeBonus", HomeBonus);
  server.on("/VisitorBonus", VisitorBonus);
  server.on("/StartStopTimer", StartStopTimer);
  server.on("/Buzzer", BuzzerStart);
  server.onNotFound(HandleNotFound);

  server.begin();
}

void loop() {
  // Eventually millis() loops back to 0
  // Avoid getting stuck by resetting everything when it happens
  if (millis() < millisCache) {
    boardUpdateTime = 0;
    lastInteraction = 0;
    buzzerStartTime = 0;
#ifdef ENABLE_LOGGING
    Serial.println("Millis Loop");
#endif
  }

  millisCache = millis();
  ScoreboardLedRoutine();
  BuzzRoutine();
  IdleRoutine();

  // call handleClient repeatedly
  server.handleClient();
  dnsServer.processNextRequest();
}

void SendWebsite() {
  server.send(200, "text/html", PAGE_MAIN);
}

void HandleNotFound() {
  server.send(200, "text/plain", "");
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
  sprintf(buf, "<Pos>%d</Pos>\n", nPossession);
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
  Serial.print("Running at ");
  Serial.println(WiFi.SSID());
  Serial.print("[");
  Serial.print(AP_SSID);
  Serial.println("]");
  Serial.println("http://scoreboard.local");
}