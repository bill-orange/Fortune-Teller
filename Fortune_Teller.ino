/* Fortune Teller using Google Cloud TTS and ChatGPT

   William E Webb (c) released under MIT license blease observe all included licenses
   
   This project uses TFT_eSPI by Bodmer The original starting point for this library was the
   Adafruit_ILI9341 library in January 2015. MIT License 
   
   This project uses ESP32-audioI2S by schreibfaul1 under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007

   This project uses ChatGPTuino by programming-electronics-acadamy 
   GNU Lesser General Public License v2.1

   This project uses button-debounce by Aaron Kimball Copyright 2022
   under BSD 3-Clause "New" or "Revised" License

   05/09/2025 First relaase on ESP32-S3 board with PSRAM and round display
              Must compile on ESP#@-S3 with OPI PSRAM enabled and huge partition scheme.

   */
#include "Arduino.h"        // Probably not necessary
#include "WiFi.h"           // Needed for ESP32
#include "Audio.h"          // The DAC pluse Google Voice
#include <HTTPClient.h>     // Get Web Data
#include <WiFiMulti.h>      // Needed for more than one possible SSDI
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // For Display
//#include <XPT2046_Touchscreen.h>  // CYD touch
#include "secrets.h"            // Passwords and keys
#include "support_functions.h"  // For PNG
#include <ChatGPTuino.h>        // For AI Support
#include <debounce.h>           // Button debounce

//SET_LOOP_TASK_STACK_SIZE(14 * 1024);  // needed to handle really long strings

/* For Cheap Yellow Display only
#define I2S_DOUT 27
#define I2S_BCLK 22
#define I2S_LRC 4
*/

/* For round display only*/
#define I2S_DOUT 21
#define I2S_BCLK 20
#define I2S_LRC 6

// Touch Screen pins
/* The CYD touch uses some non default SPI pins
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
*/

#define USE_LINE_BUFFER  // Enable for faster rendering

// Connect a button to some GPIO pin; digital pin 6 in this example.
static constexpr int PIN = 19;

const int TOKENS = 220;             // How lengthy a response you want, every token is about 3/4 a word
const int NUM_MESSAGES = 30;        // Another budget limit
const char *model = "gpt-4o-mini";  // OpenAI Model being used
//const char *model = "gpt-4o";     // OpenAI Model being used
const char *filename = "/AI_data.txt";
String Question = "";
int nexToSay = 0;
int allReadySaid1 = 0;
int allReadySaid2 = 0;
int allReadySaid3 = 0;
int audioLock = 1;


/*-------------------------------- Button Handler ----------------------------------------*/
static void buttonHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    Serial.println("Pushed button");
  } else {
    // btnState == BTN_OPEN.
    Serial.println("Released button");
    allReadySaid2 = 0;
    allReadySaid3 = 0;
    nexToSay = 1;
    audioLock = 1;
  }
}

/*-------------------------------- END Button Handler ------------------------------------*/


//audio.connecttospeech("Hello From the Great Beyond", "en");  // Google TTS
static Button myButton(0, buttonHandler);

ChatGPTuino chat{ TOKENS, NUM_MESSAGES };  // Will store and send your most recent messages (up to NUM_MESSAGES)
//XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);  // Touch screen constructor
Audio audio;
WiFiMulti wifiMulti;

/*-------------------------------- Display Graphics ----------------------------------------*/
void showGraphic(String(image)) {
  uint32_t t = millis();

  setPngPosition(0, 0);
  String githubURL = GITHUBURL + image;
  const char *URLStr = githubURL.c_str();
  Serial.println(URLStr);
  load_png(URLStr);
  t = millis() - t;
  Serial.print(t);
  Serial.println(" ms to load URL");
}
/*------------------------------------Setup -------------------------------------------------*/
void setup() {

  Serial.begin(115200);
  pinMode(PIN, INPUT_PULLUP);

  chat.init(chatGPT_APIKey, model);  // Initialize AI chat
  tft.begin();
  tft.invertDisplay(1);
  tft.fillScreen(0);
  tft.setRotation(2);
  fileInfo();
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);
  connectToWifiNetwork();
  showGraphic("WiFiConnected.png");
  delay(2000);
}

/*----------------------------------------Loop ----------------------------------------*/
void loop() {

  unsigned long timer = millis();

  if (nexToSay == 0 && allReadySaid1 == 0 && audioLock == 1) {
    delay(100);
    showGraphic("Coin_Slot_round.png");
    audio.connecttospeech("Welcome to the World of Mystery.  Press the coin slot to donate to Madam Olga", "en");  // Google TTS
    allReadySaid1 = 1;
    nexToSay = 0;
    audioLock = 0;
  }
  if (nexToSay == 1 && allReadySaid2 == 0 && audioLock == 1) {
    delay(100);
    showGraphic("Fortune_Teller.png");
    audio.connecttospeech(fortune().c_str(), "en");
    allReadySaid2 = 1;
    nexToSay = 2;
    audioLock = 0;
  }

  if (nexToSay == 2 && allReadySaid3 == 0 && audioLock == 1) {
    delay(100);
    showGraphic("Coin_Slot_round.png");
    audio.connecttospeech("Madam Olga thanks you.  Press the coin slot to donate again to Madam Olga", "en");  // Google TTS
    allReadySaid3 = 1;
    nexToSay = 3;
    audioLock = 0;
  }

  while (millis() - timer < 5000) {
    audio.loop();
    pollButtons();  // Poll your buttons every loop.
    audioLock = 1;
  }
}

/*-------------------------------- Connect to the Wifi network ------------------------------------*/
void connectToWifiNetwork() {  // Boilerplate from example (mostly)

  delay(10);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    showGraphic("error.png");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }

  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.setAutoReconnect(true);  // Reconnect if disconnected
    WiFi.persistent(true);        // Reconnect if disconnected
    //showGraphic("WiFiConnected.png", 1);
  } else {  // Handle error
    showGraphic("error.png");
    delay(6000);
  }
}

/*---------------------------- File information  ------------------------------------------*/
void fileInfo() {  // uesful to figure our what software is running

  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE);  // Print to TFT display, White color
  tft.setTextSize(1);
  tft.drawString("       Gypsy Fortune Teller", 8, 60);
  tft.drawString("       AI Fortunes", 30, 70);
  tft.setTextSize(1);
  tft.drawString(__FILENAME__, 35, 1100);
  tft.drawString(__DATE__, 35, 140);
  tft.drawString(__TIME__, 125, 140);
  delay(3000);
}

/*-------------------------------- Gets Prompt from Github -------------------------------------*/
String prompt() {  // Mostly from example
  uint32_t t = millis();
  HTTPClient http;
  http.setTimeout(120000);
  http.begin("https://raw.githubusercontent.com/bill-orange/Fortune-Teller/master/Prompt.txt");

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP ERROR: %d\n", httpCode);
    showGraphic("error.png");
    http.end();
    return "   ";
  }

  String payload = http.getString();  // Read response into a String
  http.end();                         // Close connection

  t = millis() - t;
  Serial.print(t);
  Serial.println(" ms to load URL");
  yield();
  return payload;
}

/*----------------------------------- Process AI Call ----------------------------------------*/
char *GetAIReply(char *message) {  // Function taken from libary example

  chat.putMessage(message, strlen(message));
  chat.getResponse();

  return chat.getLastMessageContent();
}

/*------------------------------- Get Fortune ------------------------------------------------*/

String fortune() {
  Serial.println("_____1____");
  String AImessage = prompt();
  Serial.println(AImessage);
  Serial.println("_____2____");
  int str_len = AImessage.length() + 1;
  char AIPrompt[str_len];
  Serial.println("_____3____");
  AImessage.toCharArray(AIPrompt, str_len);
  GetAIReply(AIPrompt);

  return String(GetAIReply(AIPrompt));
}

/*------------------------------------Button Debounce------------------------------------------*/

static void pollButtons() {
  // update() will call buttonHandler() if PIN transitions to a new state and stays there
  // for multiple reads over 25+ ms.
  myButton.update(digitalRead(PIN));
}
