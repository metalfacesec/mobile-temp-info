#include <SPI.h>
#include <Wire.h>
#include "DHT.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define TOP_MENU_HEIGHT 16
#define TOP_MENU_PADDING_TOP 4
#define LINE_HEIGHT 10

#define DHTPIN 4
#define DHTTYPE DHT11

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

bool wifiConnected = false;
const char* ssid = "XXXXX";
const char* password =  "XXXXX";
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  setupDisplay();
  setupWifi();
  dht.begin();
  
  delay(2000);
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  displayWifiStatus();
}

void setupDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  display.clearDisplay();
  display.display();
}

boolean wasHubFound(int networksFound) {
  for (int i = 0; i < networksFound; ++i) {
    String foundSSID = WiFi.SSID(i);
    if (String(ssid) == foundSSID) {
      return true;
    }
  }
  return false;
}

void connectToWifi() {
  Serial.println("Scanning for WiFi");
  
  int networksFound = WiFi.scanNetworks();
  if (networksFound == 0) {
      Serial.println("no networks found");
      return;
  }
  if(wasHubFound(networksFound)) {
    delay(10);
    initWiFi(ssid, password);
  }
}

void displayWifiStatus() {
  if (!wifiConnected) {
    writeHeader("Not Connected");
    return;
  }
  writeHeader("Connected");
}

void displayDhtReading() {
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(5000);
    return;
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, TOP_MENU_HEIGHT);
  display.println("Humidity: " + String(h));
  display.setCursor(0, TOP_MENU_HEIGHT + LINE_HEIGHT);
  display.println("Temperature: " + String(f));
}

void loop() {
  display.display();
  display.clearDisplay();
  
  if (!wifiConnected) {
    connectToWifi();
  }

  displayWifiStatus();
  displayDhtReading();
  display.display();

  delay(5000);
}

void initWiFi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  wifiConnected = true;
  Serial.println(WiFi.localIP());
}

void writeHeader(String text) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, TOP_MENU_PADDING_TOP);

  display.println(text);
}
