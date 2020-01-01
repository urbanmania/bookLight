// A simple count down timer for reading before going to bed
// 12 RGB ligts, where you can set a count down for each light of X minutes



#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <math.h>

#define pixelPin          5   // D1
#define counterButtonPin  13  // D7

uint32_t milliMinute = 60000;
uint32_t halfSec = 500000;
uint8_t blueLightLength = 10; // How many minutes each blue light is

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(12, pixelPin);

//uint32_t color  = 0xcc9900; // base litur
uint32_t color = 0x00008B;
uint32_t countcolor  = 0x0B6623; // teljari
uint32_t resetcolor  = 0xcc33cc; // reset
uint32_t delayTime = 750;
uint8_t brightnessValue = 50;

boolean counterPressed = false;
uint8_t lightCounter;
uint32_t millisecs;
uint32_t decrementTime;
uint32_t countLoops;
uint8_t cnt;

uint8_t counterButtonPinState = 1;

String header;


ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Setup");

  pixels.begin();
  for (int i = 0; i < 12; i++) {
    pixels.setPixelColor(i, color); // Base color
  }
  setBrightnessLevel();

  pinMode(counterButtonPin, INPUT);

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  server.begin();
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("You can connect to the Server here: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  server.on("/specificArgs", handleSpecificArg);
}

void loop() {

  counterButtonPinState = digitalRead(counterButtonPin);

  if (counterButtonPinState == HIGH) {
    // Counter Button is held down
    counterPressed = true;
    incrementCounter(1);

  } else {
    millisecs = millis();

    if (counterPressed) {
      counterPressed = false;
      countLoops = halfSec;
      //Serial.println(" counter was soooo Pressed ");
      // Lets re-set the counter to the number of lights
      //Serial.print("lightCounter set to: ");
      //Serial.println(lightCounter);
      decrementTime = millis() - 1000;
    }

    if (millisecs >= decrementTime && countLoops == halfSec) {
      // Liðinn tíminn, tími til að gera eitthvað

      counterPressed = false;

      showCounterColors();

      if (lightCounter > 0) lightCounter -= 1;

      decrementTime = millis() + (milliMinute * blueLightLength);

    }
    if (countLoops == halfSec) {
      countLoops = 0;
    }
    server.handleClient();
  }

}

void showCounterColors() {
  Serial.print("Liturinn er ");
  Serial.print(color);
  Serial.print(" og fjöldinn er: ");
  Serial.print(lightCounter);

  for (int i = 0; i < 12; i++) {
    pixels.setPixelColor(i, color); // Base color
  }

  for (int t = 0; t < lightCounter; t++) {
    pixels.setPixelColor(t, countcolor); // Count color
  }
  pixels.show();
}

void incrementCounter(int incrementBy) {
  lightCounter += incrementBy;

  if (lightCounter > 12) resetCounter();

  for (int t = 0; t < lightCounter; t++) {
    pixels.setPixelColor(t, countcolor); // Base color
  }
  pixels.show();
  decrementTime = (millis() + 1000) * incrementBy;
  delay(delayTime);
}

void handleSpecificArg() {

  String message = "";

  if (server.arg("numberOfLights").toInt() > 0) {   // Setting counter
    int howManyLights = server.arg("numberOfLights").toInt();

    message = "Number of lights se to: ";
    message += howManyLights;    //Gets the value of the query parameter
    message = "Brightness level % set to: ";
    message += brightnessValue;

    resetCounter();
    if (howManyLights < 1) {
      howManyLights = 1;
    } else if (howManyLights > 12) {
      howManyLights = 12;
    }
    for(int i = 0; i<howManyLights; i++) {
      incrementCounter(1);
    }
    

  }
  if (server.arg("brightness") != "") {   // Brightness

    message = "Brightness level % set to: ";
    message += server.arg("brightness");
    brightnessValue = server.arg("brightness").toInt();
    setBrightnessLevel();

  }

  if (server.arg("resetCounter") == "1") {
    message = "Counter reset"; 
    resetCounter();
  }

  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void resetCounter() {
  lightCounter = 0;
  countLoops = halfSec;
  decrementTime = millis() - 1000;
  counterPressed = false;

  for (int i = 0; i < 12; i++) {
    pixels.setPixelColor(i, color); // Base color
  }
  pixels.show();
  delay(delayTime);
  return;
}

void setBrightnessLevel() {

  if (brightnessValue < 10) {
    // Too low to register, will default to 10% brightness
    brightnessValue = 10;
  } else if (brightnessValue > 100) {
    brightnessValue = 100;
  }

  pixels.setBrightness(brightnessValue);
  pixels.show();

}
